// #include <ixwebsocket/IXWebSocket.h>
// #include <iostream>
// #include <thread>
// #include <chrono>


// int main() {

//     std::cout << "Starting server..." << std::endl;

//     ix::WebSocket webSocket;

//     webSocket.setUrl("ws://echo.websocket.org"); // שרת הדגמה שמחזיר הודעות

//     webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
//         if (msg->type == ix::WebSocketMessageType::Message) {
//             std::cout << "i get a message: " << msg->str << std::endl;
//         } else if (msg->type == ix::WebSocketMessageType::Open) {
//             std::cout << "the connection was opened!" << std::endl;
//         } else if (msg->type == ix::WebSocketMessageType::Close) {
//             std::cout << "The connection was closed." << std::endl;
//         }
//     });
//     std::cout << "Starting WebSocket..." << std::endl;

//     webSocket.start();

//     std::cout << "send a message..." << std::endl;
//     webSocket.send("hellow world!");

//     std::this_thread::sleep_for(std::chrono::seconds(5));
//     webSocket.stop();
//     std::cout << "Server finished." << std::endl;


//     return 0;
// }


#include <iostream>
#include <thread>
#include <string>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

class NetworkManager {
public:
    static bool initialize() {
#ifdef _WIN32
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
        return true;
#endif
    }
    
    static void cleanup() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

int main() {
    std::cout << "=== Cross-Platform TCP Server ===" << std::endl;
    
    if (!NetworkManager::initialize()) {
        std::cout << "Network initialization failed" << std::endl;
        return 1;
    }
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << std::endl;
        NetworkManager::cleanup();
        return 1;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed" << std::endl;
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Listen failed" << std::endl;
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    std::cout << "Server listening on port 8080..." << std::endl;
    std::cout << "Waiting for client connection..." << std::endl;
    
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Accept failed" << std::endl;
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    std::cout << "Client connected!" << std::endl;
    
    char buffer[1024];
    int messageCount = 0;
    
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "[SERVER] Received: " << buffer << std::endl;
            
            messageCount++;
            std::string response = "Server response #" + std::to_string(messageCount);
            send(clientSocket, response.c_str(), response.length(), 0);
            std::cout << "[SERVER] Sent: " << response << std::endl;
        } else if (bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        } else {
            std::cout << "Receive error" << std::endl;
            break;
        }
    }
    
    closesocket(clientSocket);
    closesocket(serverSocket);
    NetworkManager::cleanup();
    
    return 0;
}
