#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include "util/MiscUtils.h"
#include "hal/CSVHALManager.h"
#include "sensor/GPSSensor.h"

// --- Constants ---
namespace ebikeConstants {
    const std::string CONFIG_PATH = "config/client-config.yaml";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <client_ip> <ebike_id> <csv_file> <num_ports>" << std::endl;
        return 1;
    }
    
    std::string clientIp = argv[1];
    std::string ebikeId = argv[2];
    std::string csvFile = argv[3];
    int numPorts = std::atoi(argv[4]);

    int maxReadings = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "client", "max_readings"));
    int gpsSensorId = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "client", "gps_sensor_id"));
    int gpsHalPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "client", "gps_hal_port"));
    int clientPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "client", "port"));
    int mgmtPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "client", "management_port"));
    
    std::string serverIpStr = readConfigValue(ebikeConstants::CONFIG_PATH, "server", "ip");
    int serverPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "server", "port"));
    
    std::cout << "[EBCLIENT] eBike Client started on: " << clientIp << ":" << clientPort << ". Management port: " << mgmtPort << "." << std::endl;

    Poco::Net::DatagramSocket clientSock(Poco::Net::SocketAddress(Poco::Net::IPAddress(), clientPort));
    Poco::Net::SocketAddress serverAddress(serverIpStr, serverPort);
    
    // Send JOIN
    std::string joinMsg = "{\"type\":\"JOIN\",\"ebike_id\":" + ebikeId + ",\"timestamp\":\"" + getCurrentTime() + "\"}";
    clientSock.sendTo(joinMsg.c_str(), joinMsg.length(), serverAddress);
    
    int dataInterval = 5; // default fallback
    bool locked = false;

    // Wait for JACK
    char buffer[1024];
    Poco::Net::SocketAddress sender;
    // Set receive timeout so client can proceed if JACK is missed
    clientSock.setReceiveTimeout(Poco::Timespan(2, 0)); 
    try {
        int n = clientSock.receiveFrom(buffer, sizeof(buffer)-1, sender);
        if (n > 0) {
            buffer[n] = '\0';
            Poco::JSON::Parser p;
            Poco::Dynamic::Var result = p.parse(std::string(buffer));
            Poco::JSON::Object::Ptr obj = result.extract<Poco::JSON::Object::Ptr>();
            if (obj->has("data_interval")) {
                dataInterval = obj->getValue<int>("data_interval");
                std::cout << "[EBCLIENT] Joined! Sampling every " << dataInterval << "s" << std::endl;
            }
        }
    } catch (...) {
        std::cout << "[EBCLIENT] Joined! Sampling every " << dataInterval << "s (fallback)" << std::endl;
    }
    
    // Set socket to non-blocking so we don't hold up readings
    clientSock.setBlocking(false); 

    CSVHALManager halManager(numPorts);
    halManager.initialise(csvFile);

    auto gpsSensor = std::make_shared<GPSSensor>(gpsSensorId, 2);
    halManager.attachDevice(gpsHalPort, gpsSensor);

    for (int i = 0; i < maxReadings; i++) {
        // Process incoming commands
        try {
            int nMsg = clientSock.receiveFrom(buffer, sizeof(buffer)-1, sender);
            if (nMsg > 0) {
                buffer[nMsg] = '\0';
                Poco::JSON::Parser p;
                Poco::Dynamic::Var res = p.parse(std::string(buffer));
                Poco::JSON::Object::Ptr inMsg = res.extract<Poco::JSON::Object::Ptr>();
                if (inMsg->has("action")) {
                    std::string action = inMsg->getValue<std::string>("action");
                    std::cout << "[EBCLIENT] Received COMMAND: " << action << std::endl;
                    if (action == "lock") locked = true;
                    if (action == "unlock") locked = false;
                    
                    // Reply with COMMACK
                    std::string ack = "{\"type\":\"COMMACK\", \"ebike_id\":" + ebikeId + ", \"status\":\"success\"}";
                    clientSock.sendTo(ack.c_str(), ack.length(), serverAddress);
                }
                else if (inMsg->has("data_interval")) {
                     std::cout << "[EBCLIENT] Received SETUP message" << std::endl;
                     dataInterval = inMsg->getValue<int>("data_interval");
                     std::cout << "[EBCLIENT] New sampling interval: " << dataInterval << "s" << std::endl;
                }
            }
        } catch (...) {
            // No message received, continue
        }

        if (!halManager.isBusy(gpsHalPort)) {
            auto reading = halManager.read(gpsHalPort);
            if (!reading.empty()) {
                std::string rawData(reading.begin(), reading.end());
                size_t pos = rawData.find(';');
                if (pos != std::string::npos) {
                    std::string latStr = rawData.substr(0, pos);
                    std::string lonStr = rawData.substr(pos + 1);
                    std::string lockStr = locked ? "Locked" : "Unlocked";
                    int lockInt = locked ? 1 : 0;
                    std::string ts = getCurrentTime();
                    
                    std::cout << "[EBCLIENT] " << ts << " gps: {\"lat\":" << latStr << ",\"lon\":" << lonStr << "}(" << lockStr << ")" << std::endl;
                    
                    // Send DATA packet
                    std::string formattedData = "{\"type\":\"DATA\", \"ebike_id\":" + ebikeId + ", \"timestamp\":\"" + ts + "\", \"lock_status\":" + std::to_string(lockInt) + ", \"gps\": {\"lat\":" + latStr + ", \"lon\":" + lonStr + "}}";
                    clientSock.sendTo(formattedData.c_str(), formattedData.length(), serverAddress);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(dataInterval));
    }

    halManager.releaseDevice(gpsHalPort);
    std::cout << "[EBCLIENT] Shutting down." << std::endl;
    return 0;
}