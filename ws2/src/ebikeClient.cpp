#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "hal/CSVHALManager.h"
#include "sensor/GPSSensor.h"
#include "util/MiscUtils.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <server_ip> <ebike_id> <csv_file> <max_readings>"
              << std::endl;
    return 1;
  }

  std::string serverIp = argv[1];
  std::string ebikeId = argv[2];
  std::string csvFile = argv[3];
  int maxReadings = std::atoi(argv[4]);

  int serverPort = 19924;

  std::cout << "[EBCLIENT] Starting. ID: " << ebikeId << " Server: " << serverIp
            << ":" << serverPort << std::endl;

  Poco::Net::StreamSocket clientSock;
  clientSock.connect(Poco::Net::SocketAddress(serverIp, serverPort));

  // JOIN
  std::string joinMsg = "{\"type\":\"JOIN\",\"ebike_id\":" + ebikeId + "}";
  clientSock.sendBytes(joinMsg.c_str(), joinMsg.length());

  bool locked = false;
  int dataInterval = 5;
  int numPorts = 1;

  char buffer[1024];

  try {
    CSVHALManager halManager(numPorts);
    halManager.initialise(csvFile);

    auto gpsSensor = std::make_shared<GPSSensor>(0, 2);
    halManager.attachDevice(0, gpsSensor);

    for (int i = 0; i < maxReadings; i++) {
      std::cout << "[EBCLIENT] Iteration " << (i + 1) << std::endl;

      // Receive commands
      try {
        int n = clientSock.receiveBytes(buffer, sizeof(buffer) - 1);
        if (n > 0) {
          buffer[n] = '\0';

          Poco::JSON::Parser p;
          auto obj =
              p.parse(std::string(buffer)).extract<Poco::JSON::Object::Ptr>();

          if (obj->has("action")) {
            std::string action = obj->getValue<std::string>("action");

            if (action == "lock")
              locked = true;
            if (action == "unlock")
              locked = false;

            std::string ack = "{\"type\":\"COMMACK\",\"ebike_id\":" + ebikeId +
                              ",\"status\":\"success\"}";
            clientSock.sendBytes(ack.c_str(), ack.length());
          } else if (obj->has("data_interval")) {
            dataInterval = obj->getValue<int>("data_interval");
          }
        }
      } catch (...) {
      }

      // Read GPS
      auto reading = halManager.read(0);

      if (!reading.empty()) {
        std::string raw(reading.begin(), reading.end());
        size_t pos = raw.find(';');

        if (pos != std::string::npos) {
          std::string lat = raw.substr(0, pos);
          std::string lon = raw.substr(pos + 1);

          int lockVal = locked ? 1 : 0;

          std::string data = "{\"type\":\"DATA\",\"ebike_id\":" + ebikeId +
                             ",\"timestamp\":\"" + getFormattedTime() +
                             "\",\"lock_status\":" + std::to_string(lockVal) +
                             ",\"gps\":{\"lat\":" + lat + ",\"lon\":" + lon +
                             "}}";

          clientSock.sendBytes(data.c_str(), data.length());

          std::cout << "[EBCLIENT] Sent DATA: " << lat << "," << lon
                    << std::endl;
        }
      }

      std::this_thread::sleep_for(std::chrono::seconds(dataInterval));
    }

  } catch (const std::exception &e) {
    std::cerr << "[EBCLIENT] Error: " << e.what() << std::endl;
  }

  std::cout << "[EBCLIENT] Done." << std::endl;
  return 0;
}