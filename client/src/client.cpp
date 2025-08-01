// #include <ixwebsocket/IXWebSocket.h>
// #include <opencv2/opencv.hpp>
// #include "json.hpp"
// #include "gameClient.hpp"

// using json = nlohmann::json;

// // פונקציה חסרה
// std::pair<std::string, std::string> parse_event_message(const std::string& message) {
//     try {
//         json j = json::parse(message);
//         return {j["event"], j["data"]};
//     } catch (...) {
//         return {"", ""};
//     }
// }

// int main() {
//     GameClient gameClient;
//     ix::WebSocket webSocket;
    
//     webSocket.setUrl("ws://localhost:8080");
    
//     webSocket.setOnMessageCallback([&gameClient](const ix::WebSocketMessagePtr& msg) {
//         if (msg->type == ix::WebSocketMessageType::Message) {
//             auto [event, data] = parse_event_message(msg->str);
//             gameClient.on_websocket_message(event, data);
//         }
//     });
    
//     webSocket.start();
    
//     // לולאת רינדור
//     while (true) {
//         gameClient.render();
        
//         int key = cv::waitKey(50);
//         if (key == 27) break; // ESC
//         if (key != -1) {
//             gameClient.handle_input(key);
//         }
//     }
    
//     webSocket.stop();
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
    std::cout << "=== Cross-Platform TCP Client ===" << std::endl;
    
    if (!NetworkManager::initialize()) {
        std::cout << "Network initialization failed" << std::endl;
        return 1;
    }
    
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << std::endl;
        NetworkManager::cleanup();
        return 1;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    
#ifdef _WIN32
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
#else
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
    
    std::cout << "Connecting to server..." << std::endl;
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed" << std::endl;
        closesocket(clientSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    std::cout << "Connected to server!" << std::endl;
    
    char buffer[1024];
    int messageCount = 0;
    
    while (true) {
        messageCount++;
        std::string message = "Client message #" + std::to_string(messageCount);
        
        std::cout << "[CLIENT] Sending: " << message << std::endl;
        send(clientSocket, message.c_str(), message.length(), 0);
        
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "[CLIENT] Received: " << buffer << std::endl;
        } else {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        if (messageCount >= 10) {
            std::cout << "Sent 10 messages, disconnecting..." << std::endl;
            break;
        }
    }
    
    closesocket(clientSocket);
    NetworkManager::cleanup();
    
    return 0;
}
