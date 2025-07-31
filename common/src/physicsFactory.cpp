#include "physicsFactory.hpp"
#include <iostream>

PhysicsFactory::PhysicsFactory(const Board& board)
    : board_(board)
{
}

// std::unique_ptr<Physics> PhysicsFactory::create(std::pair<int,int> start_cell, const nlohmann::json& cfg) const {
//     float speed = 1.0f; // default
//     if (cfg.contains("speed_m_per_sec")) {
//         try {
//             speed = cfg["speed_m_per_sec"].get<float>();
//         } catch (...) {
//             std::cerr << "Invalid speed value in cfg\n";
//         }
//     }
//     return std::make_unique<Physics>(Cell(start_cell.first, start_cell.second), board_, speed);
// }
