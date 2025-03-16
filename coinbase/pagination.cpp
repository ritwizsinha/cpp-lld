/*

Interviewer presented a problem where there is a large dataset in backend and needs to be displayed on UI.
What are the efficient ways to fetch the data. But the answer he is looking for is Pagination.
There is a brief discussion on different methods and strategies that can be used in Pagination.
Trade-offs between different methods and which method should be choosed in which use case.
Implement 
1. limit
2. multiple filters
3. nextLink

*/

#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <map>
#include <concepts>
#include <cassert>
#include <iostream>

template<typename T>
concept Hashable = requires(T t) {
    { std::hash<decltype(t.id)>()(t.id) } -> std::convertible_to<size_t>;
};

using namespace std;
// T denotes the schema of the API


// Assuming this data is sorted and it is the index.
template<typename T>
requires Hashable<T>
class Fetcher {
    vector<T> _data;
    unordered_map<T, uint16_t> inverted_index;
    int iterIndex;
    public:
    Fetcher(vector<T> &data):_data(std::move(data)),iterIndex(0)   {
        for(int i = 0; i < _data.size(); i++) {
            inverted_index[_data[i]] = i;
        }
    }
    // Fetch as an iterator
    void resetIterator() {
        iterIndex = 0;
    }
    const T next() {
        assert(iterIndex < _data.size());
        return _data.at(iterIndex++);
    }

    const vector<T> getNextBatch(int batchsize) {
        assert(iterIndex < _data.size());
        vector<T> _results;
        for(auto i = iterIndex; batchsize > 0 and i < _data.size(); i++, batchsize--) {
            _results.push_back(_data[i]);
            iterIndex = i;
        }

        iterIndex++;

        return _results;
    }

    bool hasNext() const {
        return iterIndex < _data.size();
    }
    

    // As a page
    // limit 100 offset 100
    const vector<T> getPage(size_t limit, const size_t offset) {
        vector<T> results;
        if (offset >= _data.size()) return {};
        assert(offset < _data.size());
        for(auto index = offset; index < _data.size() and limit > 0; index++, limit--) {
           results.push_back(_data[index]); 
        }

        return results;
    }


    // As a cursor
    const vector<T> getBatch(int batchsize, T &lastItem) {
        int index = inverted_index.find(lastItem) != inverted_index.end() ? inverted_index[lastItem] : -1;
        vector<T> _results;
        for(int i = index + 1; i < _data.size() and batchsize > 0; i++, batchsize--) {
            _results.push_back(_data[i]);
        }

        return _results;
    }

    // > x
};

class DataItem {
    public:
    int id;
    int item;
    string name;
    int category;
    public:
    DataItem(int id, int item, const string &name, int category):id(id), item(item), name(name), category(category) {}
    bool operator==(const DataItem &other) const {
        return other.id == id and other.item == item and other.name == name and other.category == category;    
    }

    friend ostream& operator<<(ostream &stream, const DataItem& item) {
        stream << item.id <<" " << item.item << " " << item.name <<" " << item.category << endl;
        return stream;
    }
};


template<>
struct hash<DataItem> {
    size_t operator()(const DataItem& d) const {
        return hash<int>()(d.id);
    }
};

int main() {
    vector<DataItem> data = {{1, 2, "Something", 5}, {2, 5, "Databricks", 100}, {3, 6, "Bloomberg", 10}, {4, 100, "Wombeerg", 22}, {5, 101, "Google", 1000}, {6, 102, "Wayfair", 1022}, {7, 103, "Microsoft", 112}, {8, 104, "De Shaw", 1212}, {9, 105, "Eles", 212}};

    Fetcher<DataItem> fetcher(data);
    
    fetcher.resetIterator();
    while(fetcher.hasNext()) {
        cout << fetcher.next();
    }

    cout << endl;
    fetcher.resetIterator();
    while(fetcher.hasNext()) {
        for(auto &x: fetcher.getNextBatch(4)) {
            cout << x ;
        }
        cout << endl;
    }
    int offset = 0, limit = 2;
    while(1) {
        auto results = fetcher.getPage(limit, offset);
        if (results.empty()) break;
        for(auto &x: results) cout << x ;
        cout << endl;
        offset+= limit;
    }

    DataItem d = {-1, -1, "Starting", -1};

    while(1) {
        auto results = fetcher.getBatch(10, d);
        if (results.empty()) break;
        for(auto &x : results) cout << x ;
        cout <<endl;
        d = results.back();
    }

}
