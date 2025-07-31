#include "gameClient.hpp"

GameClient::GameClient() {
    local_publisher.subscribe(&sound_player);
    local_publisher.subscribe(&score_board);
    local_publisher.subscribe(&move_list);
    local_publisher.subscribe(&game_messages);
}

void GameClient::on_websocket_message(const std::string& event, const std::string& data) {
    local_publisher.publish(event, data);
}

void GameClient::render() {
    // renderer.set_selected_cells(selected_cell_p1, selected_cell_p2);
    // renderer.render(pieces_, board_);
}

void GameClient::handle_input(int key) {
    // העתק את כל הקוד של טיפול בקלט מהשרת לכאן
    // כולל WASD, IJKL, Space, Enter
}
