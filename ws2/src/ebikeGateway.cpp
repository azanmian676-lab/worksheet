#include "util/MiscUtils.h"
#include "web/WebServer.h"
#include "comm/SocketServer.h"
#include <iostream>
#include <Poco/Thread.h>

/***TODO: complete code as per assignment specification***/
namespace ebikeConstants {
    const std::string CONFIG_PATH = "config/server-config.yaml";
}

int main() {
    // DO NOT MODIFY THIS LINE. Creates a reference to a Poco JSON array to store the ebike objects.
    Poco::JSON::Array::Ptr ebikes = new Poco::JSON::Array();

    //** TODO Adjust & add code as per assignment specification ***/

    try {
        int webPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "webserver", "port"));
        std::string serverIp = readConfigValue(ebikeConstants::CONFIG_PATH, "server", "ip");
        int socketPort = std::stoi(readConfigValue(ebikeConstants::CONFIG_PATH, "server", "port"));
        
        // Output as requested
        std::cout << "[SOCKETS] " << serverIp << " Socket server started on:" << socketPort << ". Management port: 8085" << std::endl;

        // Create socket server
        SocketServer socketServer(socketPort, ebikes);
        Poco::Thread socketThread;
        socketThread.start(socketServer);

        // Management Socket Server on 8085
        SocketServer mgmtServer(8085, ebikes);
        Poco::Thread mgmtThread;
        mgmtThread.start(mgmtServer);

        // Create instances of the server class
        WebServer webServer(ebikes);

        // Start the server (blocks current thread)
        webServer.start(webPort);
    
        socketThread.join();
        mgmtThread.join();
        return 0;
    } catch (const Poco::Exception& ex) {
        std::cerr << "Server error (Poco): " << ex.displayText() << std::endl;
        return 1;
    }
    return 0;
}



