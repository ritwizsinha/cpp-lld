/*
Requirements
Users can post questions, answer questions, and comment on questions and answers.
Users can vote on questions and answers.
Questions should have tags associated with them.
Users can search for questions based on keywords, tags, or user profiles.
The system should assign reputation score to users based on their activity and the quality of their contributions.
The system should handle concurrent access and ensure data consistency.

Entities:
Message, Tags, Users, ReputationCalculator, StackOverflow


Message
- creator
- votes
- replies<Message>
- type <Question,Answer,Reply>
- content
- id
- vector<Tags>

User
- id
- name
- reputation

Tags
- id
- name

StackOverflow
- vector<Questions>
- postQuestion(string content) -> id
- getQuestion(id)
- replyAnswer(id)
- answer(id)
- upvote(id)
- downvote(id)


Search
    - searchByUser()
    - searchByTag()
    - searchByKeywords()
*/

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

enum class MessageType {
    QUESTION,
    ANSWER,
    REPLY
};

class Tag {
    public:
    string name;
    string id;
    Tag(const string& name, const string &id);
};


class Message {
    public:
    MessageType type;
    string content;
    vector<Message> children;
    string id;
    vector<Tag> tags;
};



class TagStore {
    private:
    TagStore() {};

    public:
    static unordered_map<string, Tag> _store;
    static unordered_map<string, Tag> _inverted_index;
    static unique_ptr<TagStore> instance;
    static once_flag initFlag;
    TagStore(const TagStore &) = delete;
    TagStore operator=(const TagStore&) = delete;

    TagStore* getTagStore() {
        call_once(initFlag, []() {
            instance = make_unique<TagStore>();
        });

        return instance.get();
    }

    static string addTag(const string& name) {
        auto id = to_string(rand());
        Tag newTag(name, id);
        _store[id] = newTag;

        return id;
    }

    static void deleteTag(const string &id) {
        if (_store.find(id) == _store.end()) return;
        _store.erase(id);
    }

    static string getId(const string &name) {
        if (_inverted_index.find(name) == _inverted_index.end()) return "";
        return _inverted_index[name].id;
    }
};

once_flag TagStore::initFlag;
unique_ptr<TagStore> TagStore::instance = nullptr;

class User {
    public:
    string name;
    string id;
    double reputation;
};
class Search {
    public:
    static vector<Message> _store;
    static vector<string> getMessages(const string &to_match) {

    }


    static void relayContent(const Message& message) {
        _store.push_back(message);
    }
};

class StackOverflow {
    public:
    using Question = Message;
    static unordered_map<string, Question> questions;
    static vector<function<void(const Message&)>> subscribers;

    static string postQuestion(const string &content) {
        Message m;
        m.content = content;
        m.type = MessageType::QUESTION;
        m.id = to_string(rand());
        
        questions[m.id] = m;
        notifySubscribers(m);
        return m.id;
    }

    static string postMessage(const string &content, const string &id, const MessageType& type) {
        Message m;
        m.content = content;
        m.id = to_string(rand());
        m.type = type;
        
        questions[id].children.push_back(m);
        notifySubscribers(m);
        return m.id;
    }

    static void notifySubscribers(const Message& message) {
        for(auto &s : subscribers) {
            s(message);
        }
    }

    static void upvote(const string &id) {
        
    }

}