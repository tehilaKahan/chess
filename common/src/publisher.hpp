#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <vector>
#include <string>

class ISubscriber;  // הצהרה מקדימה כי המשתמשים יממשו את הממשק הזה

class Publisher {
private:
    std::vector<ISubscriber*> subscribers;

public:
    void subscribe(ISubscriber* subscriber);
    void unsubscribe(ISubscriber* subscriber);
    void publish(const std::string& event_name, const std::string& data);
};

#endif // PUBLISHER_H
