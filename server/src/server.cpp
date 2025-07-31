#include <ixwebsocket/IXWebSocket.h>
#include <iostream>

int main() {
    ix::WebSocket webSocket;

    webSocket.setUrl("ws://echo.websocket.org"); // שרת הדגמה שמחזיר הודעות

    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            std::cout << "קיבלתי הודעה: " << msg->str << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "החיבור נפתח!" << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "החיבור נסגר." << std::endl;
        }
    });

    webSocket.start();

    std::cout << "שולח הודעה..." << std::endl;
    webSocket.send("שלום עולם!");

    std::this_thread::sleep_for(std::chrono::seconds(5));
    webSocket.stop();

    return 0;
}
