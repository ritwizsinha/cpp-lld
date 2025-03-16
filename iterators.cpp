#include <vector>
#include <string>
#include <cassert>
#include <list>
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename T>
class BaseIterator {
    public:
    virtual bool hasNext() = 0;
    virtual T next() = 0;
    ~BaseIterator() {}
};

template<typename T>
class ZigZagIterator : public BaseIterator<T> {
    using IteratorList = list<shared_ptr<BaseIterator<T>>>;
    IteratorList iterators;
    IteratorList::iterator primary_iterator;

    public:
    ZigZagIterator(vector<shared_ptr<BaseIterator<T>>> &iterators) {
        for(auto &iter : iterators) {
            if (iter.get()->hasNext()) iterators.push_back(move(iter));
        }
    }


    bool hasNext() override {
        return iterators.size() > 0;
    }


    T next() {
        int value = (*primary_iterator)->next();
        if (!(*primary_iterator)->hasNext()) primary_iterator = iterators.erase(primary_iterator);
        else primary_iterator++;
        
        if (primary_iterator == iterators.end()) primary_iterator = iterators.begin();
    }
};


class OddIterator : public BaseIterator<int> {
    int current;
    public:
    OddIterator() {
        current = 0;
    }
    bool hasNext() {
        return true;
    }

    int next() {
        int value = current;
        current += 2;
        return value;
    }
};

class EvenIterator: public BaseIterator<int> {
    OddIterator iterator;
    public:
    EvenIterator(const OddIterator &iterator):iterator(iterator) {}

    bool hasNext() override {
        return iterator.hasNext();
    }

    int next() override {
        return iterator.next() + 1;
    }
};

class RangeIterator : public BaseIterator<int> {
    int start;
    int end;
    int current;
    int increment;
    public:
    RangeIterator(const int start, const int end, int increment):start(start), end(end), increment(increment), current(start) {}
    bool hasNext() override {
        return current <= end;
    }

    int next() override {
        int value = current;
        current += increment;
        return value;
    }
};

// template<typename T>
// class ZigZagIterator {
//     using Iterator = vector<T>::iterator;
//     using End = vector<T>::iterator;
//     using IteratorList = list<pair<Iterator, End>>;
//     public:
//     ZigZagIterator(const vector<pair<Iterator, End>> &iterators) {
//         // if ()
//         for(auto &[curIter, endIter] : iterators) {
//             if (curIter == endIter) continue;
//             else _itrs.push_back(make_pair(curIter, endIter));
//         }


//         _primaryIterator = _itrs.begin();
//     }


//     bool hasNext() {
//         return _itrs.size() > 0;
//     }

//     T next() {
//         auto &[curIter, curIterEnd] = *_primaryIterator;
//         auto value = *curIter;
        
//         if (++curIter == curIterEnd) _primaryIterator = _itrs.erase(_primaryIterator);
//         else _primaryIterator++;
        
//         if (_primaryIterator == _itrs.end()) _primaryIterator = _itrs.begin();
//         return value;
//     }





//     private:
//     IteratorList _itrs;
//     IteratorList::iterator _primaryIterator;
// };


// class RangeIterator {
//     public:
//     RangeIterator(const int start, const int end, const int increment):start(start),end(end),increment(increment), initialized(false) {}

//     bool hasNext() {
//         if (!initialized) return true;
//         return cur + increment <= end;
//     }

//     bool hasPrevious() {
//         if (!initialized) return true;
//         return cur - increment >= start;
//     }

//     int getNext() {
//         if (!initialized) {
//             initialized = true;
//             return cur = start;
//         }
//         return cur += increment;
//     }

//     int getPrevious() {
//         if (!initialized) {
//             initialized = true;
//             return cur = start;
//         }
//         return cur -= increment;
//     }


//     private:

//     int start;
//     int end;
//     int increment;
//     int cur;
//     bool initialized;

// };
template<typename T>
class ListIterator {
    vector<T> _store;
    vector<T>::iterator _itr;
    public:
    ListIterator(vector<T> &list):_store(move(list)),_itr(_store.begin()) {
    
    }

    bool hasNext() {
        return _itr != _store.end();
    }


    T getNext() {
        return *_itr++;
    }
};


struct Item {
    int cost;
    int weight;
    Item(int cost, int weight):cost(cost), weight(weight) {}
};


class Knapsack {
    public:
    Knapsack(const vector<Item> &items, int Weight):items(std::move(items)), capacity(Weight) {}

    int solve() {
        vector<int> maxCost(capacity + 1, 0);
        for(int weight = 1; weight <= capacity; weight++) {
            for(auto &[cost, w] : items) {
                if (weight >= w) maxCost[weight] = max(maxCost[weight], maxCost[weight - w] + cost);
            }
        }


        return maxCost[capacity];
    }



    private:
    vector<Item> items;
    int capacity;
};

class LogParser {
    public:
    void notify(const string &message) {
        parse(message);
    }


    private:
    void parse(const string &message) {
        regex pattern(R"(\[.*?\] ERROR: (.*))");
        smatch match;
        if (regex_search(message, match, pattern)) {
            cout << "Found message " << match[1] << endl;
        }

    }
};



// class Arbitrarge {
//     private:
//     vector<pair<string, pair<string, double>>> edges; 
//     unordered_map<string, unordered_map<string, double>> adj;
//     unordered_set<string> unique;
//     public:
//     void processString(const string &s) {
//         string cur_pair;
//         stringstream ss(s);
//         while(getline(ss, cur_pair, ';')) {
//             int stop_index = cur_pair.find(',');
//             string cur1 = string(begin(cur_pair), begin(cur_pair) + stop_index);
//             string cur2 = string(begin(cur_pair) + stop_index + 1, begin(cur_pair) + cur_pair.find(':'));
//             unique.insert(cur1);
//             unique.insert(cur2);
//             string fx = string(begin(cur_pair) + cur_pair.find(':') + 1, end(cur_pair));
//             // cout << cur1 <<" " << cur2 <<"  " << fx << endl;
//             edges.push_back(make_pair(cur1, make_pair(cur2, stod(fx))));
//             adj[cur1][cur2] = stod(fx);
//             adj[cur2][cur1] = 1.0 / stod(fx);
//         }
//     }
//     public:
//     // USD,BTC:4.5;DOGE,USD
//     Arbitrarge(const string &s) {
//         processString(s);
//     }


//     double findMaxBacktracking(const string &currency1, const string currency2) {
//         double maxProfit = -1e19;
//         auto dfs = [&](const string &cur_currency, double fx, unordered_set<string> &visited, auto &self) -> void {
//             if (cur_currency == currency2) {
//                 maxProfit = max(maxProfit, fx);
//                 return;
//             }

//             for(auto &[ch, nfx]: adj[cur_currency]) {
//                 if (visited.find(ch) == visited.end()) {
//                     visited.insert(ch);
//                     self(ch, fx * nfx, visited, self);
//                     visited.erase(ch);
//                 }
//             }
//         };

//         unordered_set<string> visited;
//         dfs(currency1, 1, visited, dfs);


//         return maxProfit;
//     }

//     double findMaxBellmanFord(string &currency1, string &currency2) {
//         double maxProfit = -1e19;

//         int n = unique.size();
//         unordered_map<string, double> distances;
//         for(auto &x: unique) distances[x] = INFINITY;
//         distances[currency1] = 1;

//         for(int i = 0; i < n - 1; i++) {
//             for(auto &[c1, node] : edges) {
//                 auto &[c2, price] = node;
//                 if (distances[c1] < INFINITY) {
//                     distances[c2] = distances[c1] * (-price);
//                 }
//             }
//         }

//         return abs(distances[currency2]);
//     }

//     vector<string> getRandomTwo() {
//         vector<string> r(begin(unique), end(unique));
//         int x, y;
//         do {
//             x = rand() % r.size();
//             y = rand() % r.size();
//         } while(x == y);

//         return {r[x], r[y]};
//     }

//     static void m() {
//         string currency_data[] = {
//         "USD,EUR:0.85;EUR,USD:1.18;GBP,INR:100.50;JPY,USD:0.009;AUD,CAD:0.94;",
//         "USD,EUR:0.87;USD,EUR:0.83;GBP,USD:1.38;GBP,USD:1.40;GBP,USD:1.35;INR,USD:0.012;",
//         "USD,EUR:0.85;JPY,USD:0.009;USD,GBP:0.72;CAD,USD:0.79;USD,AUD:1.29;BTC,USD:45000.00;",
//         "USD,BTC:0.000022;ETH,USD:3200.00;USD,ETH:0.00031;LTC,USD:150.00;USD,LTC:0.0067;",
//         "EUR,JPY:129.53;JPY,EUR:0.0077;AUD,INR:55.10;INR,AUD:0.018;CAD,GBP:0.59;GBP,CAD:1.69;",
//         "USD,CHF:0.92;CHF,USD:1.09;SGD,HKD:5.80;HKD,SGD:0.172;NZD,USD:0.65;USD,NZD:1.54;",
//         "USD,MXN:19.85;MXN,USD:0.0504;EUR,GBP:0.85;GBP,EUR:1.18;JPY,CAD:0.011;CAD,JPY:90.90;",
//         "SGD,USD:0.74;USD,SGD:1.35;HKD,JPY:14.23;JPY,HKD:0.0703;INR,EUR:0.0115;EUR,INR:87.00;",
//         "AUD,NZD:1.07;NZD,AUD:0.93;CNY,USD:0.14;USD,CNY:7.15;RUB,USD:0.013;USD,RUB:76.92;",
//         "CHF,EUR:1.02;EUR,CHF:0.98;GBP,AUD:1.85;AUD,GBP:0.54;BTC,ETH:14.1;ETH,BTC:0.071;",
//         "USD,ZAR:14.56;ZAR,USD:0.0686;BRL,USD:0.19;USD,BRL:5.25;TRY,USD:0.054;USD,TRY:18.52;",
//         "KRW,USD:0.00075;USD,KRW:1333.33;SEK,USD:0.096;USD,SEK:10.42;NOK,USD:0.095;USD,NOK:10.52;",
//         "DKK,USD:0.15;USD,DKK:6.85;PLN,USD:0.24;USD,PLN:4.18;HUF,USD:0.0031;USD,HUF:320.00;",
//         "CZK,USD:0.045;USD,CZK:22.22;AED,USD:0.27;USD,AED:3.67;SAR,USD:0.27;USD,SAR:3.75;",
//         "MYR,USD:0.23;USD,MYR:4.35;THB,USD:0.029;USD,THB:34.48;IDR,USD:0.00007;USD,IDR:14285.71;",
//         "PKR,USD:0.0064;USD,PKR:156.25;EGP,USD:0.064;USD,EGP:15.63;NGN,USD:0.0024;USD,NGN:416.67;",
//         "KES,USD:0.0091;USD,KES:109.89;TZS,USD:0.00043;USD,TZS:2330.00;GHS,USD:0.17;USD,GHS:5.88;",
//         "BTC,USDT:45000.00;USDT,BTC:0.000022;ETH,USDT:3200.00;USDT,ETH:0.00031;LTC,USDT:150.00;USDT,LTC:0.0067;"
//         };

//         for(auto &currency: currency_data) {
//             Arbitrarge arg(currency);
//             auto r = arg.getRandomTwo();
//             cout << r[0] <<" " << r[1] << " " <<arg.findMaxBacktracking(r[0], r[1]) <<" " << arg.findMaxBellmanFord(r[0], r[1]) << endl;
//         }
    
//     }
// };

int main() {
    // Arbitrarge::m();
    auto odd = make_shared<OddIterator>();
    auto even = make_shared<EvenIterator>(*odd.get());

    auto rangeIterator = make_shared<RangeIterator>(0, 100, 20);
    vector<shared_ptr<BaseIterator<int>>> results;
    results.push_back(odd);
    results.push_back(even);
    results.push_back(rangeIterator);
    ZigZagIterator<int> zigzag(results);

    // while(zigzag->)

}
