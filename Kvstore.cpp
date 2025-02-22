#include <vector>
#include <stack>
#include <iostream>
#include <list>
using namespace std;


struct TrieNode {
    vector<unique_ptr<TrieNode>> children;
    bool eow;
    size_t timestamp;
    int val;
    TrieNode():children(26), eow(false),timestamp(INT_MAX),val(0) {
    }
    TrieNode(int val):children(26),eow(false),timestamp(INT_MAX),val(val) {
    }

    TrieNode(int val, int timestamp):children(26), eow(false),timestamp(timestamp),val(val) {
    }

};

struct Value {
    int val;
    bool has_value;
    Value(int val):val(val), has_value(true) {}
    Value():has_value(false) {}
    Value(int val, bool has_value): val(val), has_value(has_value) {}
};

struct oper {
    int optype;
    int val;
    int ttl;
};
struct Trie {
    list<oper> undo;
    list<oper> redo;
    list<oper>::iterator undoItr, redoItr;
    unique_ptr<TrieNode> node;
    Trie() : node(make_unique<TrieNode>()), undoItr(undo.begin()), redoItr(redo.begin())  {
    }


    void insert(const string &key, int val, int timestamp = INT_MAX) {
        auto cur = node.get();

        for(int i = 0; i < key.size(); i++) {
            if (cur->children[key[i] - 'a'] == nullptr) cur->children[key[i] - 'a'] = make_unique<TrieNode>();
            cur = cur->children[key[i] - 'a'].get();
        }

        cur->eow = true;
        cur->val = val;
        cur->timestamp = timestamp;
        undo.insert(undoItr, oper{2, -1, -1});
        redo.insert(redoItr, oper{2, val, timestamp});   
        undoItr++;
        redoItr++;
    }

    Value search(const string &key, int timestamp = 0) {
        auto cur = node.get();
        for(int i = 0; i < key.size(); i++) {
            if (cur->children[key[i] - 'a'] == nullptr) return Value();
            cur = cur->children[key[i] - 'a'].get();
        }

        return Value(cur->val, cur->timestamp > timestamp);
    }


    void remove(const string &key) {
        if (!search(key).has_value)  return;

        stack<pair<int,TrieNode*>> st;
        auto cur = node.get();
        for(int i = 0; i < key.size(); i++) {
            st.push({key[i] - 'a', cur});
            cur = cur->children[key[i] - 'a'].get();
        }

        int lastIndex = key.back() - 'a';
        while(st.size()) {
            auto [idx, parent] = st.top();
            st.pop();
            if (idx != lastIndex and parent->children[idx].get()->eow) break;
            bool anotherExists = false;
            for(int i = 0; i < 26; i++) {
                if (parent->children[i].get() != nullptr and i != idx) anotherExists = true;
            }
            if (anotherExists) break;
            parent->children[idx] = nullptr;
        }

        
    }

    vector<Value> prefixSearch(const string &pref) {
       vector<Value> results;
       auto parse = [&](TrieNode* cur, int index, auto &self) -> void {
        if (index < pref.size()) cout << pref[index] <<  " " << index << endl;
            if (index < pref.size()) {
                self(cur->children[pref[index] - 'a'].get(), index + 1, self);
                return ;
            }
            if (!cur) return;
            if (cur->eow) {
                results.push_back(Value(cur->val));
            }
            for(int i = 0; i < 26; i++) {
                self(cur->children[i].get(), index, self);
            }
       };

       parse(node.get(), 0, parse);

       return results;
    }
};





int main() {
Trie trie;
trie.insert("ritwiz", 1);
trie.insert("anjali", 2);
trie.insert("anjan", 3);
trie.insert("akash", 100);
trie.insert("rit", 4);
trie.remove("akash");
trie.remove("ritwiz");
// cout << trie.search("rit").val << endl;
// trie.prefixSearch("anj");
for(auto &x: trie.prefixSearch("")) cout << x.val << endl;
}