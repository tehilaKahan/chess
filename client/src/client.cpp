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

#include <opencv2/opencv.hpp>
#include "gameClient.hpp"
#include "../../common/src/cell.hpp"
#include "../../common/src/command.hpp"
#include <sstream>
#include <algorithm>

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

class NetworkedChessClient {
private:
    SOCKET socket;
    GameClient gameClient;
    
    // משתני UI לבחירת חיילים
    Cell selected_cell_p1 {6,0};
    Cell selected_cell_p2 {0,0};
    bool selecting_source_p1 = true;
    bool selecting_source_p2 = true;
    Cell src_p1, dst_p1;
    Cell src_p2, dst_p2;
    std::string selected_piece_id_p1;
    std::string selected_piece_id_p2;
    
public:
    NetworkedChessClient(SOCKET s) : socket(s) {}
    
    void sendCommand(const std::string& piece_id, const Cell& src, const Cell& dst) {
        std::string message = "MOVE:" + piece_id + ":" + 
                             std::to_string(src.row) + ":" + std::to_string(src.col) + ":" +
                             std::to_string(dst.row) + ":" + std::to_string(dst.col);
        
        send(socket, message.c_str(), message.length(), 0);
        std::cout << "[CLIENT] Sent command: " << message << std::endl;
    }
    
    void processServerMessage(const std::string& message) {
        if (message.substr(0, 6) == "EVENT:") {
            // פרסור אירוע מהשרת
            size_t firstColon = message.find(':', 6);
            if (firstColon != std::string::npos) {
                std::string event = message.substr(6, firstColon - 6);
                std::string data = message.substr(firstColon + 1);
                
                // העברת האירוע ל-GameClient
                gameClient.on_websocket_message(event, data);
            }
        }
    }
    
    void handleInput(int key) {
        gameClient.handle_input(key);
        
        // טיפול בקלט אמיתי - כמו במשחק המקורי
        auto clamp_cell = [](Cell& c) {
            c.row = std::clamp(c.row, 0, 7);
            c.col = std::clamp(c.col, 0, 7);
        };
        
        // שחקן 1 (לבן) - תנועה (WASD)
        if (key == 'w' || key == 'W') {
            selected_cell_p1.row--;
            clamp_cell(selected_cell_p1);
        } else if (key == 's' || key == 'S') {
            selected_cell_p1.row++;
            clamp_cell(selected_cell_p1);
        } else if (key == 'a' || key == 'A') {
            selected_cell_p1.col--;
            clamp_cell(selected_cell_p1);
        } else if (key == 'd' || key == 'D') {
            selected_cell_p1.col++;
            clamp_cell(selected_cell_p1);
        }
        // שחקן 2 (שחור) - תנועה (IJKL)
        else if (key == 'i' || key == 'I') {
            selected_cell_p2.row--;
            clamp_cell(selected_cell_p2);
        } else if (key == 'k' || key == 'K') {
            selected_cell_p2.row++;
            clamp_cell(selected_cell_p2);
        } else if (key == 'j' || key == 'J') {
            selected_cell_p2.col--;
            clamp_cell(selected_cell_p2);
        } else if (key == 'l' || key == 'L') {
            selected_cell_p2.col++;
            clamp_cell(selected_cell_p2);
        }
        // שחקן 1 - בחירה (רווח)
        else if (key == 32) {
            if (selecting_source_p1) {
                src_p1 = selected_cell_p1;
                selected_piece_id_p1 = "PW_pos_" + std::to_string(src_p1.row) + "_" + std::to_string(src_p1.col);
                selecting_source_p1 = false;
                std::cout << "[CLIENT] Selected source: " << selected_piece_id_p1 << std::endl;
            } else {
                dst_p1 = selected_cell_p1;
                // שלח פקודה אמיתית!
                sendCommand(selected_piece_id_p1, src_p1, dst_p1);
                selecting_source_p1 = true;
            }
        }
        // שחקן 2 - בחירה (אנטר)
        else if (key == 13) {
            if (selecting_source_p2) {
                src_p2 = selected_cell_p2;
                selected_piece_id_p2 = "PB_pos_" + std::to_string(src_p2.row) + "_" + std::to_string(src_p2.col);
                selecting_source_p2 = false;
                std::cout << "[CLIENT] Selected source: " << selected_piece_id_p2 << std::endl;
            } else {
                dst_p2 = selected_cell_p2;
                // שלח פקודה אמיתית!
                sendCommand(selected_piece_id_p2, src_p2, dst_p2);
                selecting_source_p2 = true;
            }
        }
    }
    
    void render() {
        gameClient.render();
    }
};

void receiveMessages(SOCKET socket, NetworkedChessClient* client) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            client->processServerMessage(std::string(buffer));
        } else {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
    }
}

int main() {
    std::cout << "=== Networked Chess Client ===" << std::endl;
    
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
    
    std::cout << "Connecting to chess server..." << std::endl;
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed" << std::endl;
        closesocket(clientSocket);
        NetworkManager::cleanup();
        return 1;
    }
    
    std::cout << "Connected to chess server!" << std::endl;
    
    NetworkedChessClient chessClient(clientSocket);
    
    // התחלת thread לקבלת הודעות
    std::thread receiveThread(receiveMessages, clientSocket, &chessClient);
    
    // לולאת המשחק הראשית (כמו בקוד המקורי)
    while (true) {
        chessClient.render();
        
        int key = cv::waitKey(50);
        if (key == 27) break; // ESC
        if (key != -1) {
            chessClient.handleInput(key);
        }
    }
    
    receiveThread.join();
    
    closesocket(clientSocket);
    NetworkManager::cleanup();
    
    return 0;
}
