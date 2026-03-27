/**
 * @file MessageHandler.h
 * @brief Handles all incoming UDP messages for the eBike Gateway.
 *
 * Implements the BASIC and ENHANCED protocol directives:
 *   JOIN   - eBike registers with gateway; receives JACK acknowledgement.
 *   DATA   - eBike sends GPS + lock status; stored as GeoJSON for the web map.
 *   COMMAND - Management client locks/unlocks specific eBikes.
 *   COMMACK - eBike confirms a COMMAND was applied.
 *   SETUP   - Management client updates the data-send interval for all eBikes.
 */

#pragma once
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/StreamSocket.h>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <mutex>

/**
 * @class MessageHandler
 * @brief Static handler that routes incoming JSON messages to the correct protocol action.
 *
 * A shared eBike client registry (keyed by ebike_id) records the sender address
 * of every JOIN so that COMMAND and SETUP directives can be forwarded correctly.
 */
class MessageHandler {
private:
    /** Registry mapping ebike_id -> active TCP StreamSocket session. */
    static std::map<int, Poco::Net::StreamSocket> ebikeClients;
    /** Mutex protecting ebikeClients and the ebikes shared array. */
    static std::mutex registryMutex;

public:
    /**
     * @brief Parse and dispatch an incoming UDP message.
     *
     * @param msg        Raw JSON string received from the socket.
     * @param ebikes     Shared GeoJSON array displayed by the web server.
     * @param socket     Socket used to send responses/forwards.
     * @param senderUrl  Address of the UDP packet originator.
     */
    static void handleMessage(const std::string& msg,
                              Poco::JSON::Array::Ptr ebikes,
                              Poco::Net::StreamSocket& socket,
                              const Poco::Net::SocketAddress& senderUrl) {
        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(msg);
            Poco::JSON::Object::Ptr jsonObj = result.extract<Poco::JSON::Object::Ptr>();

            if (!jsonObj->has("type")) {
                return; // Ignore malformed or unknown messages
            }

            std::string msgType = jsonObj->getValue<std::string>("type");

            // ---------------------------------------------------------------
            // JOIN — eBike registers with the gateway
            // ---------------------------------------------------------------
            if (msgType == "JOIN") {
                int ebikeId = jsonObj->getValue<int>("ebike_id");

                {
                    std::lock_guard<std::mutex> lock(registryMutex);
                    ebikeClients[ebikeId] = socket;

                    // [NEW] Manually Register in ebikes array for immediate tracking
                    bool replaced = false;
                    for (size_t i = 0; i < ebikes->size(); i++) {
                        if (ebikes->getObject(i)->getObject("properties")->getValue<int>("id") == ebikeId) {
                            replaced = true; break;
                        }
                    }
                    if (!replaced) {
                        Poco::JSON::Object::Ptr placeholder = new Poco::JSON::Object();
                        placeholder->set("type", "Feature");
                        Poco::JSON::Object::Ptr geom = new Poco::JSON::Object();
                        geom->set("type", "Point");
                        Poco::JSON::Array::Ptr coords = new Poco::JSON::Array();
                        coords->add(0.0); coords->add(0.0); // Placeholder coords
                        geom->set("coordinates", coords);
                        placeholder->set("geometry", geom);
                        Poco::JSON::Object::Ptr props = new Poco::JSON::Object();
                        props->set("id", ebikeId);
                        props->set("status", "joined");
                        placeholder->set("properties", props);
                        ebikes->add(placeholder);
                    }
                }
                std::cout << "[SOCKETS] eBike " << ebikeId << " joined" << std::endl;
                std::cout << "[DEBUG] Total tracked ebikes: " << ebikes->size() << std::endl;

                std::string jack = "{\"status\":\"success\", \"data_interval\":5}";
                socket.sendBytes(jack.c_str(), jack.length());
            }

            // ---------------------------------------------------------------
            // DATA — eBike sends GPS + lock status reading
            // ---------------------------------------------------------------
            else if (msgType == "DATA") {
                int ebikeId        = jsonObj->getValue<int>("ebike_id");
                std::string timestamp = jsonObj->getValue<std::string>("timestamp");
                int lock_status    = jsonObj->has("lock_status")
                                     ? jsonObj->getValue<int>("lock_status") : 0;

                Poco::JSON::Object::Ptr gps = jsonObj->getObject("gps");
                double lat = gps->getValue<double>("lat");
                double lon = gps->getValue<double>("lon");

                std::string lockStr = (lock_status == 1) ? "locked" : "unlocked";

                std::cout << "[MHANDLER] DATA from " << ebikeId
                          << " at " << timestamp
                          << ": lat=" << lat << ", lon=" << lon
                          << " (" << lockStr << ")" << std::endl;

                // Build a GeoJSON Feature for the Leaflet web map
                Poco::JSON::Object::Ptr geojson  = new Poco::JSON::Object();
                geojson->set("type", "Feature");

                Poco::JSON::Object::Ptr geometry = new Poco::JSON::Object();
                geometry->set("type", "Point");
                Poco::JSON::Array::Ptr coords = new Poco::JSON::Array();
                coords->add(lon);
                coords->add(lat);
                geometry->set("coordinates", coords);
                geojson->set("geometry", geometry);

                // Use key "id" to match map.html's ebike.properties.id lookup
                Poco::JSON::Object::Ptr props = new Poco::JSON::Object();
                props->set("id", ebikeId);
                props->set("timestamp", timestamp);
                props->set("status", lockStr);
                geojson->set("properties", props);

                // Replace existing entry or append new one
                std::lock_guard<std::mutex> lock(registryMutex);
                bool replaced = false;
                for (size_t i = 0; i < ebikes->size(); i++) {
                    Poco::JSON::Object::Ptr existing = ebikes->getObject(i);
                    if (existing->getObject("properties")->getValue<int>("id") == ebikeId) {
                        ebikes->set(i, geojson);
                        replaced = true;
                        break;
                    }
                }
                if (!replaced) ebikes->add(geojson);
            }

            // ---------------------------------------------------------------
            // COMMAND — Management client issues lock / unlock for target eBikes
            // ---------------------------------------------------------------
            else if (msgType == "COMMAND") {
                std::string action = jsonObj->getValue<std::string>("action");
                std::cout << "[MHANDLER] COMMAND from management client: action="
                          << action << std::endl;

                // Forward COMMAND to each targeted eBike client
                Poco::JSON::Array::Ptr targetIds = jsonObj->getArray("ebike_ids");
                if (targetIds) {
                    std::lock_guard<std::mutex> lock(registryMutex);
                    for (size_t i = 0; i < targetIds->size(); i++) {
                        int targetId = targetIds->getElement<int>(i);
                        auto it = ebikeClients.find(targetId);
                        if (it != ebikeClients.end()) {
                            // Send action to the eBike client
                            std::string cmd = "{\"action\":\"" + action
                                            + "\", \"ebike_id\":" + std::to_string(targetId) + "}";
                            try {
                                it->second.sendBytes(cmd.c_str(), cmd.length());
                            } catch (...) {
                                std::cerr << "[MHANDLER] Failed to forward COMMAND to " << targetId << std::endl;
                            }
                        }
                    }
                }

                // Acknowledge receipt to the management client
                std::string ack = "{\"ebike_id\": -1, \"status\": \"success\"}";
                socket.sendBytes(ack.c_str(), ack.length());
            }

            // ---------------------------------------------------------------
            // COMMACK — eBike confirms it applied the COMMAND
            // ---------------------------------------------------------------
            else if (msgType == "COMMACK") {
                int ebikeId        = jsonObj->getValue<int>("ebike_id");
                std::string status = jsonObj->getValue<std::string>("status");
                std::cout << "[MHANDLER] COMMACK from ebike " << ebikeId
                          << ": " << status << std::endl;
            }

            // ---------------------------------------------------------------
            // SETUP — Management client updates the data-send interval
            // ---------------------------------------------------------------
            else if (msgType == "SETUP") {
                int interval = jsonObj->getValue<int>("data_interval");
                std::cout << "[MHANDLER] SETUP: interval updated to " << interval << std::endl;

                // Broadcast new interval to every registered eBike client
                {
                    std::lock_guard<std::mutex> lock(registryMutex);
                    std::string setupMsg = "{\"data_interval\":" + std::to_string(interval) + "}";
                    for (auto& kv : ebikeClients) {
                        try {
                            kv.second.sendBytes(setupMsg.c_str(), setupMsg.length());
                        } catch (...) {}
                    }
                }

                // Acknowledge to management client
                std::string ack = "{\"status\": \"success\"}";
                socket.sendBytes(ack.c_str(), ack.length());
            }

        } catch (Poco::Exception& e) {
            std::cerr << "[MHANDLER] Parse error: " << e.message() << std::endl;
        }
    }
};

// C++17 inline static member definitions (header-only)
inline std::map<int, Poco::Net::StreamSocket> MessageHandler::ebikeClients;
inline std::mutex MessageHandler::registryMutex;

#endif // MESSAGEHANDLER_H
