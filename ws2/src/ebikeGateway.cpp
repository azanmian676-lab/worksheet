#include "comm/SocketServer.h"
#include "util/MiscUtils.h"
#include "web/WebServer.h"
#include <Poco/Thread.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <string>
#include <map>

/***TODO: complete code as per assignment specification***/
namespace ebikeConstants {
const std::string CONFIG_PATH = "config/server-config.yaml";
}

// Local helper to read config without crashing
std::string getLocalConfig(const std::string& section, const std::string& key) {
    std::ifstream file(ebikeConstants::CONFIG_PATH);
    if (!file.is_open()) return "";
    
    std::string line;
    bool inSection = false;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        // Check if this is a top-level section (starts with no spaces and ends with :)
        if (line.find(":") != std::string::npos && line[0] != ' ') {
            inSection = (line.find(section + ":") == 0);
            continue;
        }
        
        if (inSection && line.find(key + ":") != std::string::npos) {
            size_t colonPos = line.find(":");
            std::string val = line.substr(colonPos + 1);
            size_t hashPos = val.find("#");
            if (hashPos != std::string::npos) val = val.substr(0, hashPos);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);
            return val;
        }
    }
    return "";
}

int main() {
  std::cout << "[GATEWAY] main() started" << std::endl;
  // DO NOT MODIFY THIS LINE.
  Poco::JSON::Array::Ptr ebikes = new Poco::JSON::Array();
  std::cout << "[GATEWAY] ebikes array created" << std::endl;

  try {
    std::cout << "[GATEWAY] Loading config via fallback..." << std::endl;
    std::string webPortStr = getLocalConfig("webserver", "port");
    if (webPortStr.empty()) webPortStr = "8090"; // Fallback
    int webPort = std::stoi(webPortStr);

    std::string serverIp = getLocalConfig("server", "ip");
    if (serverIp.empty()) serverIp = "127.0.0.1";

    std::string socketPortStr = getLocalConfig("server", "port");
    if (socketPortStr.empty()) socketPortStr = "8080";
    int socketPort = std::stoi(socketPortStr);
    std::string mgmtPortStr = getLocalConfig("server", "management_port");
    if (mgmtPortStr.empty()) mgmtPortStr = "11185";
    int mgmtPort = std::stoi(mgmtPortStr);

    // Output as requested
    std::cout << "[SOCKETS] " << serverIp << " Socket server started on:" << socketPort
              << ". Management port: " << mgmtPort << std::endl;

    // Create socket server
    SocketServer socketServer(socketPort, ebikes);
    Poco::Thread socketThread;
    socketThread.start(socketServer);

    // Management Socket Server
    SocketServer mgmtServer(mgmtPort, ebikes);
    Poco::Thread mgmtThread;
    mgmtThread.start(mgmtServer);

    // Create instances of the server class
    WebServer webServer(ebikes);

    // Start the server (blocks current thread)
    webServer.start(webPort);

    socketThread.join();
    mgmtThread.join();
    return 0;
  } catch (const Poco::Exception &ex) {
    std::cerr << "Server error (Poco): " << ex.displayText() << std::endl;
    return 1;
  } catch (const std::exception &ex) {
    std::cerr << "Server error (std): " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
