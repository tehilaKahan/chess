#include "img.hpp"
#include <iostream>
#include "board.hpp"
#include "piece.hpp"
#include "state.hpp"
#include "moves.hpp"
#include "graphics.hpp"
#include "physics.hpp"
#include "game.hpp"
#include "physicsFactory.hpp"
#include "command.hpp"
#include "mockImg.hpp"
#include <filesystem>
#include <memory>
#include <filesystem>
#include "gameFactory.hpp"


namespace fs = std::filesystem;




int main() {
    Game game = create_game("../../board.png", "../../pieces");
    game.run();
    return 0;
}