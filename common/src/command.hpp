#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include "cell.hpp"
#include <memory>


// מייצג פקודה אחת במשחק
struct Command {
    int timestamp;         // זמן ב־ms מתחילת המשחק
    std::string piece_id;  // מזהה של החייל
    std::string type;      // סוג הפקודה ("Move", "Jump", ...)
    Cell src;              // משבצת מקור
    Cell dst;              // משבצת יעד
    
    Command();
    Command(int time, const std::string& id, const std::string& t, const Cell& s, const Cell& d);
    bool is_empty() const {
        return type.empty();
    }
    std::string to_string() const;

};

#endif // COMMAND_H
