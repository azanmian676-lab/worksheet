#pragma once
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "MessageHandler.h"
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <iostream>
#include <string>
#include <thread>

class SocketServer : public Poco::Runnable {
private:
  int port;
  Poco::JSON::Array::Ptr ebikes;

public:
  SocketServer(int port, Poco::JSON::Array::Ptr ebikes)
      : port(port), ebikes(ebikes) {}

  void run() override {
    try {
      Poco::Net::ServerSocket serverSocket(port);

      while (true) {
        Poco::Net::StreamSocket clientSocket = serverSocket.acceptConnection();

        std::thread([this, clientSocket]() mutable {
          try {
            char buffer[2048];

            while (true) {
              int n = clientSocket.receiveBytes(buffer, sizeof(buffer) - 1);
              if (n <= 0)
                break;

              buffer[n] = '\0';
              std::string msg(buffer);

              MessageHandler::handleMessage(msg, ebikes, clientSocket,
                                            clientSocket.peerAddress());
            }
          } catch (...) {
            std::cout << "[SOCKETS] Client disconnected" << std::endl;
          }
        }).detach();
      }
    } catch (Poco::Exception &e) {
      std::cerr << "[SOCKETS] TCP Server error: " << e.displayText()
                << std::endl;
    }
  }
};

#endif