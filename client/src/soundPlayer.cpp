#include "soundPlayer.hpp"
#include <iostream>

SoundPlayer::SoundPlayer() {
    // טוענים קבצי סאונד פעם אחת לפי אירועים
    if (!buffers_["move"].loadFromFile("C:/CTD25/cpp/audio/move.wav")) {
        std::cerr << "Could not load move.wav\n";
    }
    if (!buffers_["capture"].loadFromFile("C:/CTD25/cpp/audio/capture.wav")) {
        std::cerr << "Could not load capture.wav\n";
    }
    if(!buffers_["jump"].loadFromFile("C:/CTD25/cpp/audio/jump.wav")) {
        std::cerr << "Could not load jump.wav\n";
    }
    if (!buffers_["win"].loadFromFile("C:/CTD25/cpp/audio/gameend.mp3")) {
        std::cerr << "Could not load win.wav\n";
    }
    if(!buffers_["fail"].loadFromFile("C:/CTD25/cpp/audio/fail.mp3")) {
        std::cerr << "Could not load fail.wav\n";
    }
    if(!buffers_["start"].loadFromFile("C:/CTD25/cpp/audio/gamestart.mp3")) {
        std::cerr << "Could not load start.wav\n";
    }
}

void SoundPlayer::on_event(const std::string& event_name, const std::string& data) {
    if (event_name == "move_started") {
        sound_.setBuffer(buffers_["move"]);
        sound_.play();
    } else if (event_name == "piece_captured") {
        sound_.setBuffer(buffers_["capture"]);
        sound_.play();
    }else if (event_name == "jump") {
        sound_.setBuffer(buffers_["jump"]);
        sound_.play();
    } else if (event_name == "game_won") {
        sound_.setBuffer(buffers_["win"]);
        sound_.play();
    } else if (event_name == "failed") {
        sound_.setBuffer(buffers_["fail"]);
        sound_.play();
    } else if (event_name == "game_started") {
        sound_.setBuffer(buffers_["start"]);
        sound_.play();
    } else {
        std::cerr << "Unknown event: " << event_name << std::endl;
    }
}
