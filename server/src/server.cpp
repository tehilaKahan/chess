// #include <iostream>
// #include <thread>
// #include <string>
// #include <chrono>

// #ifdef _WIN32
//     #include <winsock2.h>
//     #include <ws2tcpip.h>
//     #pragma comment(lib, "ws2_32.lib")
//     typedef int socklen_t;
// #else
//     #include <sys/socket.h>
//     #include <netinet/in.h>
//     #include <arpa/inet.h>
//     #include <unistd.h>
//     #define SOCKET int
//     #define INVALID_SOCKET -1
//     #define SOCKET_ERROR -1
//     #define closesocket close
// #endif

// class NetworkManager {
// public:
//     static bool initialize() {
// #ifdef _WIN32
//         WSADATA wsaData;
//         return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
// #else
//         return true;
// #endif
//     }
    
//     static void cleanup() {
// #ifdef _WIN32
//         WSACleanup();
// #endif
//     }
// };

// int main() {
//     std::cout << "=== Cross-Platform TCP Server ===" << std::endl;
    
//     if (!NetworkManager::initialize()) {
//         std::cout << "Network initialization failed" << std::endl;
//         return 1;
//     }
    
//     SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == INVALID_SOCKET) {
//         std::cout << "Socket creation failed" << std::endl;
//         NetworkManager::cleanup();
//         return 1;
//     }
    
//     sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     serverAddr.sin_port = htons(8080);
    
//     if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
//         std::cout << "Bind failed" << std::endl;
//         closesocket(serverSocket);
//         NetworkManager::cleanup();
//         return 1;
//     }
    
//     if (listen(serverSocket, 1) == SOCKET_ERROR) {
//         std::cout << "Listen failed" << std::endl;
//         closesocket(serverSocket);
//         NetworkManager::cleanup();
//         return 1;
//     }
    
//     std::cout << "Server listening on port 8080..." << std::endl;
//     std::cout << "Waiting for client connection..." << std::endl;
    
//     SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
//     if (clientSocket == INVALID_SOCKET) {
//         std::cout << "Accept failed" << std::endl;
//         closesocket(serverSocket);
//         NetworkManager::cleanup();
//         return 1;
//     }
    
//     std::cout << "Client connected!" << std::endl;
    
//     char buffer[1024];
//     int messageCount = 0;
    
//     while (true) {
//         int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//         if (bytesReceived > 0) {
//             buffer[bytesReceived] = '\0';
//             std::cout << "[SERVER] Received: " << buffer << std::endl;
            
//             messageCount++;
//             std::string response = "Server response #" + std::to_string(messageCount);
//             send(clientSocket, response.c_str(), response.length(), 0);
//             std::cout << "[SERVER] Sent: " << response << std::endl;
//         } else if (bytesReceived == 0) {
//             std::cout << "Client disconnected" << std::endl;
//             break;
//         } else {
//             std::cout << "Receive error" << std::endl;
//             break;
//         }
//     }
    
//     closesocket(clientSocket);
//     closesocket(serverSocket);
//     NetworkManager::cleanup();
    
//     return 0;
// }


#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

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

#include "../../common/src/game.hpp"
#include "../../common/src/gameFactory.hpp"

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

class NetworkedChessServer {
private:
    std::unique_ptr<Game> game;
    SOCKET clients[2];
    std::thread gameThread;
    
public:
    NetworkedChessServer() {
        std::cout << "[DEBUG] NetworkedChessServer constructor started" << std::endl;
        clients[0] = INVALID_SOCKET;
        clients[1] = INVALID_SOCKET;
        
        try {
            // יצירת משחק ברירת מחדל פשוט
            std::cout << "[DEBUG] Creating empty pieces vector" << std::endl;
            std::vector<std::shared_ptr<Piece>> empty_pieces;
            
            std::cout << "[DEBUG] Creating empty board" << std::endl;
            Board empty_board(100, 100, 8, 8, nullptr);
            
            std::cout << "[DEBUG] Creating Game object" << std::endl;
            game = std::make_unique<Game>(empty_pieces, empty_board);
            
            std::cout << "[SERVER] Game created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "[ERROR] Exception in constructor: " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cout << "[ERROR] Unknown exception in constructor" << std::endl;
            throw;
        }
        std::cout << "[DEBUG] NetworkedChessServer constructor finished" << std::endl;
    }
    
    void setClients(SOCKET client1, SOCKET client2) {
        clients[0] = client1;
        clients[1] = client2;
        
        // הגדרת callback לשליחת עדכונים לקליינטים
        if (game) {
            game->set_websocket_callback([this](const std::string& event, const std::string& data) {
                broadcastToAll("EVENT:" + event + ":" + data);
            });
        }
    }
    
    void startGame() {
        // הרצת המשחק בthread נפרד
        if (game) {
            gameThread = std::thread([this]() {
                game->run();
            });
        }
    }
    
    void processClientCommand(const std::string& message, int playerNum) {
        std::cout << "[SERVER] Player " << playerNum << " sent: " << message << std::endl;
        
        if (message.substr(0, 5) == "MOVE:") {
            // פרסור הפקודה: MOVE:piece_id:src_row:src_col:dst_row:dst_col
            std::vector<std::string> parts;
            std::stringstream ss(message);
            std::string item;
            
            while (std::getline(ss, item, ':')) {
                parts.push_back(item);
            }
            
            if (parts.size() >= 6) {
                std::string piece_id = parts[1];
                Cell src(std::stoi(parts[2]), std::stoi(parts[3]));
                Cell dst(std::stoi(parts[4]), std::stoi(parts[5]));
                
                // יצירת פקודה
                Command cmd(0, piece_id, "move", src, dst); // השתמש ב-0 כ-timestamp זמני
                
                // שליחת הפקודה למשחק
                if (game) {
                    game->add_command(cmd);
                }
                
                std::cout << "[SERVER] Added command: " << cmd.to_string() << std::endl;
            }
        }
    }
    
    void broadcastToAll(const std::string& message) {
        for (int i = 0; i < 2; i++) {
            if (clients[i] != INVALID_SOCKET) {
                send(clients[i], message.c_str(), message.length(), 0);
                std::cout << "[SERVER] Broadcasted to client " << (i+1) << ": " << message << std::endl;
            }
        }
    }
    
    ~NetworkedChessServer() {
        if (gameThread.joinable()) {
            gameThread.join();
        }
    }
};

void handleClient(SOCKET clientSocket, NetworkedChessServer* server, int playerNum) {
    char buffer[1024];
    
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string message(buffer);
            server->processClientCommand(message, playerNum);
        } else {
            std::cout << "Player " << playerNum << " disconnected" << std::endl;
            break;
        }
    }
}

int main() {
    std::cout << "=== Networked Chess Game Server ===" << std::endl;
    
    if (!NetworkManager::initialize()) {
        std::cout << "Network initialization failed" << std::endl;
        std::cin.get(); // המתן לפני סגירה
        return 1;
    }
    
    std::cout << "Network initialized successfully" << std::endl;
    
    std::cout << "Creating chess server..." << std::endl;
    NetworkedChessServer chessServer;
    std::cout << "Chess server created successfully" << std::endl;
    
    std::cout << "Creating socket..." << std::endl;
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << std::endl;
        std::cin.get(); // המתן לפני סגירה
        NetworkManager::cleanup();
        return 1;
    }
    std::cout << "Socket created successfully" << std::endl;
    
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
    
    if (listen(serverSocket, 2) == SOCKET_ERROR) {
        std::cout << "Listen failed" << std::endl;
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    std::cout << "Chess server listening on port 8080..." << std::endl;
    std::cout << "Waiting for 2 players to connect..." << std::endl;
    
    // קבלת שני קליינטים
    SOCKET client1 = accept(serverSocket, nullptr, nullptr);
    if (client1 == INVALID_SOCKET) {
        std::cout << "Failed to accept client 1" << std::endl;
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    std::cout << "Player 1 connected!" << std::endl;
    
    SOCKET client2 = accept(serverSocket, nullptr, nullptr);
    if (client2 == INVALID_SOCKET) {
        std::cout << "Failed to accept client 2" << std::endl;
        closesocket(client1);
        closesocket(serverSocket);
        NetworkManager::cleanup();
        return 1;
    }
    std::cout << "Player 2 connected!" << std::endl;
    
    chessServer.setClients(client1, client2);
    chessServer.startGame();
    
    // יצירת threads לטיפול בקליינטים
    std::thread thread1(handleClient, client1, &chessServer, 1);
    std::thread thread2(handleClient, client2, &chessServer, 2);
    
    thread1.join();
    thread2.join();
    
    closesocket(client1);
    closesocket(client2);
    closesocket(serverSocket);
    NetworkManager::cleanup();
    
    return 0;
}
