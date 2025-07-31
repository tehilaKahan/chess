#pragma once
#include <ixwebsocket/IXWebSocket.h>
#include "../../common/src/publisher.hpp"
#include "soundPlayer.hpp"
#include "scoreBoard.hpp"
#include "moveList.hpp"
#include "../../common/src/gameMessages.hpp"
#include "gameRenderer.hpp"
#include <ixwebsocket/IXWebSocket.h>



class GameClient {
public:
    GameClient();
    void on_websocket_message(const std::string& event, const std::string& data);
    void render();
    void handle_input(int key);


private:
    Publisher local_publisher;
    GameRenderer renderer;
    SoundPlayer sound_player;
    ScoreBoard score_board;
    MoveList move_list;
    GameMessages game_messages;

    // משתני UI
    Cell selected_cell_p1 {6,0};
    Cell selected_cell_p2 {0,0};
    bool selecting_source_p1 = true;
    bool selecting_source_p2 = true;
};
