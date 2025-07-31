#include <SFML/Audio.hpp>
#include <iostream>

int main() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("C:\\Users\\tehil\\Downloads\\TADA.WAV")) {
        std::cerr << "Failed to load sound\n";
        return 1;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();

    while (sound.getStatus() == sf::Sound::Playing) {
        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}
