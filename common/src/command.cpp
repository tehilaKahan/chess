#include "command.hpp"



// Command

Command::Command() : timestamp(0), piece_id(""), type(""), src(), dst() {}
// command.cpp
std::string Command::to_string() const {
    return "type=" + type +
           ", piece_id=" + piece_id +
           ", from=(" + std::to_string(src.row) + "," + std::to_string(src.col) + ")" +
           ", to=(" + std::to_string(dst.row) + "," + std::to_string(dst.col) + ")";
}

Command::Command(int time, const std::string& id, const std::string& t, const Cell& s, const Cell& d)
    : timestamp(time), piece_id(id), type(t), src(s), dst(d) {}
