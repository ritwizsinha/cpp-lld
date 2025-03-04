/*
4:15
5:30
1. Billions of words and create searches based on keyword
2. Create a keyword index larger than memory
3. Store index and metadata
4. Index a message
5. Search with a keyword
6. Search with multiple keywords


Message, Word or Keyword, Index, IndexMetadata, IndexManager, FileManager


Message1 -> id
k1 -> vector<ids>
k2 -> vector<ids>

FileManager:
- FileManager(base_dir)
- storeToFile(unordered_map<string, vector<ids>>) -> vector<keyword, offset>, file
- mergeToFiles(file1, file2) ->

IndexManager
- addWordToIndex: Add to a unordered and persist to disk using Filemanager if size is >= max_size, empty the memory
- getWordsForIndex
- unordered_map<string, vector<ids>>
- vector<string> sequenceNumbers;
- IndexMetadata


keyword -> vector<pair<f1, offset>>>
IndexPersistence
- storeToFile() -> updates metadata about keywords
- merge()
*/

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cassert>

using namespace std;
namespace fs = filesystem;

class Utilities {
    public:
    static vector<int> parseSeqNums(const string &line) {
        stringstream ss(line);
        string l;
        getline(ss, l, ':');
        vector<int> answers;
        while(getline(ss, l, ',')) {
            answers.push_back(stoi(l));
        }
        
        return answers;
    }
};

class FileManager {
    private:
    string base_dir;
    public:
    FileManager(const string &base_dir) :base_dir(base_dir) {
        if (!fs::exists(base_dir)) {
            fs::create_directory(base_dir);
        }
    };

    vector<pair<string,int>> storeToFile(const string &filename, const unordered_map<string, vector<int>>& mem_store) {
        const string path = base_dir + '/' + filename + ".txt";
        ofstream outfile(path);
        int cur_offset = 0;
        vector<pair<string,int>> offsets;
        for(auto &[key, value] : mem_store) {
            offsets.push_back({key, cur_offset});
            outfile << key <<":";
            bool first = false;
            for(auto &v : value) {
                if (first) outfile << ",";
                first = true;
                outfile << v;
            }
            outfile << "\n";
            cur_offset++;
        }
        
        outfile.close();

        return offsets;
    };

    string readOffset(const string &filename, int offset) {
        const string path = base_dir + '/' + filename + ".txt";
        ifstream f(path);
        string line;
        int cur_line = 0;
        while(getline(f, line)) {
            if (cur_line == offset) return line;
            cur_line++;
        }

        return "";
    }


};


class IndexManager {
    private:
    vector<string> sequenceNumbers;
    unordered_map<string, vector<int>> mem_store;
    unique_ptr<FileManager> manager;
    unordered_set<int> filenames;
    // Map a keyword to a file and an offset.
    unordered_map<string, vector<pair<int,int>>> disk_cache;
    int capacity;
    void updateDiskCache(int filename, vector<pair<string, int>> &offsets) {
        for(auto &[key, offset]: offsets) {
            disk_cache[key].push_back({filename, offset});
        }
    }
    public:
    IndexManager(int capacity): capacity(capacity), manager(make_unique<FileManager>("Index")) {}
    string getBySequenceNumber(int seq_num) {
        return sequenceNumbers[seq_num];
    };

    void addWord(const string &message) {
        int cur_seq_num = sequenceNumbers.size();
        sequenceNumbers.push_back(message);
        stringstream ss(message);
        string keyword;
        while(getline(ss, keyword, ' ')) {
            mem_store[keyword].push_back(cur_seq_num);
        }
        
        if (mem_store.size() >= capacity) {
            const string filename = to_string(rand());
            filenames.insert(stoi(filename));
            auto offsets = manager->storeToFile(filename, mem_store);   
            updateDiskCache(stoi(filename), offsets);
            mem_store.clear();
        }
    }


    vector<string> search(const string &keyword) {
        vector<string> answers;
        for(auto &v : mem_store[keyword]) {
            answers.push_back(sequenceNumbers[v]);
        }
        for(auto &[f, offset] : disk_cache[keyword]) {
            auto line = manager->readOffset(to_string(f), offset);
            vector<int> sns = Utilities::parseSeqNums(line);
            for(auto &x : sns) {
                answers.push_back(sequenceNumbers[x]);
            }

        }

        return answers;
    }

    vector<string> search(const vector<string> &keywords) {
        int total_keywords = keywords.size();
        unordered_map<string, int> freq;
        for(auto &keyword : keywords) {
            for(auto &[f, offset] : disk_cache[keyword]) {
                vector<int> sns = Utilities::parseSeqNums(manager->readOffset(to_string(f), offset));
                for(auto &x : sns) {
                    freq[sequenceNumbers[x]]++;
                }
    
            } 
        } 
        vector<string> answers;
        for(auto &[k,v] : freq) {
            if (v == total_keywords)
                answers.push_back(k);
        }

        return answers;
    }
};




int main() {
    IndexManager i(2); // Setting a small capacity to trigger file writes quickly

    // Adding sample messages
    i.addWord("Hello my name is Alice");
    i.addWord("Hello my name is Bob");
    i.addWord("Hola is the Hello in Spanish");
    i.addWord("Bonjour is the Hello in French");
    i.addWord("Alice loves data structures");
    i.addWord("Bob enjoys system design");

    // Test 1: Searching for "Hello" should return multiple results
    auto helloResults = i.search("Hello");
    assert(helloResults.size() == 4);
    assert(helloResults[0] == "Hello my name is Alice");
    assert(helloResults[1] == "Hello my name is Bob");
    assert(helloResults[2] == "Hola is the Hello in Spanish");
    assert(helloResults[3] == "Bonjour is the Hello in French");

    // Test 2: Searching for "French" should return 1 result
    auto frenchResults = i.search("French");
    assert(frenchResults.size() == 1);
    assert(frenchResults[0] == "Bonjour is the Hello in French");

    // Test 3: Searching for "Alice" should return 2 messages
    auto aliceResults = i.search("Alice");
    assert(aliceResults.size() == 2);
    assert(aliceResults[0] == "Hello my name is Alice");
    assert(aliceResults[1] == "Alice loves data structures");

    // Test 4: Searching for "Bob" should return 2 messages
    auto bobResults = i.search("Bob");
    assert(bobResults.size() == 2);
    assert(bobResults[0] == "Hello my name is Bob");
    assert(bobResults[1] == "Bob enjoys system design");

    // Test 5: Multi-keyword search - "Alice" and "Hello"
    auto multiSearch = i.search(vector<string>({"Alice", "Hello"}));
    assert(multiSearch.size() == 1);
    assert(multiSearch[0] == "Hello my name is Alice");

    // Test 6: Searching for a non-existing keyword
    auto nonexistentResults = i.search("NonExistingKeyword");
    assert(nonexistentResults.empty());

    // Test 7: Verifying persistence - ensure data can be retrieved from disk
    auto spanishResults = i.search("Spanish");
    assert(spanishResults.size() == 1);
    assert(spanishResults[0] == "Hola is the Hello in Spanish");

    // If all tests pass
    cout << "✅ All test cases passed!" << endl;

    return 0;
}