#include "Publisher.hpp"
#include <algorithm> // ל־std::find ו־std::remove
#include "ISubscriber.hpp" // כאן מגדירים את הממשק Observer

void Publisher::subscribe(ISubscriber* subscriber) {
    if (std::find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end()) {
        subscribers.push_back(subscriber);
    }
}

void Publisher::unsubscribe(ISubscriber* subscriber) {
    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), subscriber), subscribers.end());
}

void Publisher::publish(const std::string& event_name, const std::string& data) {
    for (auto* subscriber : subscribers) {
        subscriber->on_event(event_name, data);
    }
}
