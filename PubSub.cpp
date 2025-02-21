/*
Pub Sub System

1. Publish messages to certain topics
2. Subscribers should subscribe to topic of interest and and receive messages published to those topics
3.  Multiple publishers and subscribers
4. Messages delivered in realtime
5. Concurrent access and thread safety


Entities
Publishers, LogStore, Topic, Subscribers, Messages

1 Publisher -> 1 Topic
1 Topic -> N subscribers

*/
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

using namespace std;


struct Message {
    string message;
    int id;
    Message(const string &msg): message(msg),id(rand() % 1000) {}
};


struct Topic {
    Topic(const string &name): name(name) {}
    using Callback = function<void(const Message&msg)>;

    string name;
    unordered_map<string, Callback> subscribers;
    void addSub(const string &subscriber, Callback callback) {
        if (subscribers.find(subscriber) != subscribers.end()) return;
        subscribers[subscriber] = callback;
    }

    void removeSub(const string &subscriber) {
        if (subscribers.find(subscriber) == subscribers.end()) return;
        subscribers.erase(subscriber);
    }

    void notify(const Message& msg) {
        for(auto &[k, cb]: subscribers) {
            cb(msg);
        }
    }
    void publish(const Message& msg) {
        notify(msg);
    }

};

int main() {
    Topic topic("main topic");

    topic.addSub("subscriber 1", [](const Message &msg) -> void {
        cout << "Subscriber 1 " <<msg.id << " " << msg.message << endl;
    });

    topic.addSub("subscriber 2", [](const Message &msg) -> void {
        cout << "Subscriber 2 " <<msg.id << " " << msg.message << endl;
    });
    
    Message m("This is a new message");
    topic.publish(m);

    Message m2("This is a new message 2");
    topic.publish(m2);
}

/*
Function callbacks can be used instead of function pointers. 
Function callbacks are more flexible and can be used with any function that matches the signature of the callback. 
Function pointers are more limited and can only be used with functions that have the same signature as the function pointer.



*/