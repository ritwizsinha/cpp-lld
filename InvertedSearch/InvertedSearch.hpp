#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <thread>
#include <memory>
#include <chrono>
#include <fstream>

using namespace std;

// database
class Database
{
    static vector<string> texts;

public:
    static int insert(const string &text)
    {
        texts.push_back(text);
        return texts.size() - 1;
    }

    static string find(const int &id)
    {
        if (id < texts.size())
            return texts[id];

        return "";
    }
};


class DiskOrchestrator;

class InvertedSearch
{
    static void init() {
        DiskOrchestrator::init();
    }
    static unordered_map<string, vector<int>> index;
    static int count;
    InvertedSearch() {}

    static void insert(const std::string &text)
    {
        int id = Database::insert(text);
        stringstream ss(text);
        string keyword;
        while (getline(ss, keyword, ' '))
        {
            index[keyword].push_back(id);
        }
    }

    static vector<string> search(const std::string &keyword)
    {
        vector<string> results;
        if (index.find(keyword) != index.end())
        {
            for (int i = 0; i < index[keyword].size(); i++)
            {
                results.push_back(Database::find(index[keyword][i]));
            }
        }

        return results;
    }

    static vector<string> search(const vector<string> &keywords)
    {
        vector<string> answers;
        
        unsigned int cores = thread::hardware_concurrency();
    }
};

class DiskOrchestrator
{
    public:
    static void init()
    {
        mergeTogether();
    }
    static vector<string> filenames;
    static int fileNumber;

    static void flush(unordered_map<string, vector<int>> &index)
    {
        string filename = "file" + to_string(fileNumber++);
        ofstream f(filename, ios::app);
        if (f.is_open())
        {
            for (auto &[k, v] : index)
            {
                f << "|" << k;
                for (auto &wordIndex : v)
                    f << "," << wordIndex;
                f << "\n";
            }
            f.close();
        }
        filenames.push_back(filename);
    }

    static void mergeTogether()
    {
        auto merge = [&]() -> void
        {
            auto itr = end(filenames);
            auto fileA = *(--itr);
            auto fileB = *(--itr);

            ofstream fa(fileA, ios::app);
            ifstream fb(fileB);
            string line;
            while (getline(fb, line))
            {
                fa << line;
            };
            fa.close();
            fb.close();
        };

        auto doForever = [&]() -> void
        {
            while (true)
            {
                if (filenames.size() > 2)
                    merge();
                this_thread::sleep_for(chrono::seconds(10));
            }
        };

        thread(doForever).detach();
    }
};

int DiskOrchestrator::fileNumber = 0;
// struct TrieNode;

// class FileHandler {
//     unique_ptr<TrieNode> base;
//     static unsigned int LIMIT;
//     FileHandler() : base(make_unique<TrieNode>()) {
//         for (char c = 'a'; c <= 'z'; c++) {
//             auto node = base.get();
//             node->terminal = false;
//             node->children.emplace_back(make_unique<TrieNode>());
//             node->children.back().get()->filename = node->filename + to_string(c);
//         }
//     }

//     static void flush(unordered_map<string, vector<int>> &index) {
//         for(auto &[keyword, list]: index) {

//         }
//     }
// };

// // struct TrieNode {
//     vector<unique_ptr<TrieNode>> children;
//     string filename = "";
//     int filesize;
//     bool terminal;
//     TrieNode(): terminal(true), filesize(0) {}
//     static string findFilename(const string &keyword, unique_ptr<TrieNode> root) {
//         int index = 0;
//         TrieNode* node = root.get();
//         while(node and !node->terminal) {
//             node = node->children[keyword[index++] - 'a'].get();
//         }

//         if (!node) return "";
//         else return node->filename;
//     }
// };

unsigned int LIMIT = (1 << 8) * 4000 // 4 KB.

    int InvertedSearch::count = 0;