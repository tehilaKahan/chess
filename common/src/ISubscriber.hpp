#ifndef ISUBSCRIBER_HPP
#define ISUBSCRIBER_HPP

#include <string>

class ISubscriber {
public:
    virtual ~ISubscriber() = default;

    // הפונקציה שקוראים לה כשיש אירוע חדש
    virtual void on_event(const std::string& event_name, const std::string& data) = 0;
};

#endif // ISUBSCRIBER_HPP
