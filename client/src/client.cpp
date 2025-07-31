#include <ixwebsocket/IXWebSocket.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include "gameClient.hpp"

using json = nlohmann::json;

// פונקציה חסרה
std::pair<std::string, std::string> parse_event_message(const std::string& message) {
    try {
        json j = json::parse(message);
        return {j["event"], j["data"]};
    } catch (...) {
        return {"", ""};
    }
}

int main() {
    GameClient gameClient;
    ix::WebSocket webSocket;
    
    webSocket.setUrl("ws://localhost:8080");
    
    webSocket.setOnMessageCallback([&gameClient](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            auto [event, data] = parse_event_message(msg->str);
            gameClient.on_websocket_message(event, data);
        }
    });
    
    webSocket.start();
    
    // לולאת רינדור
    while (true) {
        gameClient.render();
        
        int key = cv::waitKey(50);
        if (key == 27) break; // ESC
        if (key != -1) {
            gameClient.handle_input(key);
        }
    }
    
    webSocket.stop();
    return 0;
}
