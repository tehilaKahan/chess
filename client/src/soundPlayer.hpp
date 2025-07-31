#ifndef SOUND_PLAYER_HPP
#define SOUND_PLAYER_HPP

#include "../../common/src/ISubscriber.hpp"
#include <SFML/Audio.hpp>
#include <map>
#include <string>

class SoundPlayer : public ISubscriber {
public:
    SoundPlayer();
    void on_event(const std::string& event_name, const std::string& data) override;

private:
    std::map<std::string, sf::SoundBuffer> buffers_;
    sf::Sound sound_;
};

#endif // SOUND_PLAYER_HPP
