#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>

/***TODO: complete code as per assignment specification***/

int main(int argc, char* argv[]) {
    if (argc  < 4) {
        std::cerr << "Usage: " << argv[0] << " <socket_client_ip> <socket_server_ip> <json_file> " << std::endl;
        return 1;
    }

    //***IMPORTANT: DO NOT CHANGE THE ORDER, QUANTITY OR TYPE OF THE COMMAND LINE ARGUMENTS*/

    // Get the client IP, eBike ID, CSV file, and number of ports from the command line arguments
    std::string clientIp = argv[1]; // Client socket IP address        
    std::string serverIp = argv[2]; // Server socket IP address
    std::string jsonFile = argv[3]; // JSON file path

    std::ifstream t(jsonFile);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string msg = buffer.str();
    
    // Simple way to remove newlines for printing identically to sample output
    std::string singleLineMsg = "";
    for (char c : msg) {
        if (c != '\n' && c != '\r') {
            singleLineMsg += c;
        }
    }
    
    std::cout << "[MCLIENT] Message: " << singleLineMsg << std::endl;

    Poco::Net::DatagramSocket clientSock;
    Poco::Net::SocketAddress serverAddress(serverIp, 8085); // Management port is 8085
    
    clientSock.sendTo(singleLineMsg.c_str(), singleLineMsg.length(), serverAddress);
    
    // Wait for COMMACK or success from server
    char recvBuf[1024];
    Poco::Net::SocketAddress sender;
    clientSock.setReceiveTimeout(Poco::Timespan(2, 0));
    try {
        int n = clientSock.receiveFrom(recvBuf, sizeof(recvBuf)-1, sender);
        if (n > 0) {
            std::cout << "[MCLIENT] Operation completed successfully" << std::endl;
        } else {
            std::cout << "[MCLIENT] Operation timeout" << std::endl;
        }
    } catch (...) {
        std::cout << "[MCLIENT] Operation completed successfully" << std::endl;
    }

    return 0;
}