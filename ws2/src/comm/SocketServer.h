#pragma once
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <string>
#include <iostream>
#include "MessageHandler.h"

class SocketServer : public Poco::Runnable {
private:
    Poco::Net::DatagramSocket socket;
    Poco::JSON::Array::Ptr ebikes;
    int port;
public:
    SocketServer(int port, Poco::JSON::Array::Ptr ebikes) : ebikes(ebikes), port(port) {
        socket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress(), port));
        // Only print main port explicitly starting
        // Note: The Management port might be a different one, but assuming gateway handles both or unified, we print it via gateway.
    }
    
    void run() override {
        char buffer[2048];
        while (true) {
            Poco::Net::SocketAddress sender;
            int n = socket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
            if (n > 0) {
                buffer[n] = '\0';
                std::string msg(buffer);
                MessageHandler::handleMessage(msg, ebikes, socket, sender);
            }
        }
    }
};

#endif
