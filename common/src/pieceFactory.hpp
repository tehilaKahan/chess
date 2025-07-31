#ifndef PIECE_FACTORY_HPP
#define PIECE_FACTORY_HPP

#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <utility>
#include <tuple>

#include "board.hpp"
#include "piece.hpp"
#include "state.hpp"
#include "graphicsFactory.hpp"
#include "physicsFactory.hpp"

namespace fs = std::filesystem;

class PieceFactory {
public:
    PieceFactory(const Board& board, const fs::path& pieces_root);


    // מחזיר shared_ptr ל-Piece עם גרפיקה ופיזיקה משויכים
    std::shared_ptr<Piece> create_piece(const std::string& p_type, std::pair<int,int> cell);

private:
std::shared_ptr<State> PieceFactory::build_state_machine(const fs::path& piece_dir);

    const Board& board_;
    fs::path pieces_root_;

    std::unordered_map<std::string, std::shared_ptr<State>> state_templates_;

    GraphicsFactory graphics_factory_;
    PhysicsFactory physics_factory_;
};

#endif // PIECE_FACTORY_HPP
