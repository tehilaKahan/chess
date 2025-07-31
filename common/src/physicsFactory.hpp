#ifndef PHYSICS_FACTORY_HPP
#define PHYSICS_FACTORY_HPP

#include "board.hpp"
#include "physics.hpp"
#include <memory>
#include <map>
#include <string>
#include "json.hpp"
class PhysicsFactory {
public:
    explicit PhysicsFactory(const Board& board);

    // יצירת Physics בהתאם לתא התחלה ו"קונפיג" (ניתן להחליף map לסוג אחר לפי הצורך)
std::unique_ptr<Physics> create(std::pair<int,int> start_cell, const nlohmann::json& cfg) const ;


private:
    const Board& board_;
};

#endif // PHYSICS_FACTORY_HPP
