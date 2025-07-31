// #include "moveList.hpp"

// void MoveList::on_event(const std::string& event_name, const std::string& data) {
//     // נכנסים רק לאירוע move
//     if (event_name == "move") {
//         moves.push_back(data);
//         // אופציונלי: הדפסת ל־stdout לצורך טיפול ראשוני / דיבוג
//         // std::cout << "MoveList: תור חדש – " << data << std::endl;
//     }
// }


// עדכון moveList.cpp
#include "moveList.hpp"
#include <iostream>


void MoveList::on_event(const std::string& event_name, const std::string& data) {
    if (event_name == "move_started") {
        moves.push_back(data);
        if (moves.size() > 50) { // שמירה על מקסימום 50 מהלכים
            moves.erase(moves.begin());
        }
    }
}
