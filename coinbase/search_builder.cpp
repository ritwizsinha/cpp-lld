// You will be given a list of transactions. You need to write a generic search api to filter on a combination of specific fields and values
// Example :
// transactions = [
// {id: 1, time: 1, userId: 1, amount: 10},
// {id: 2, time: 2, userId: 3, amount: 10},
// {id: 3, time: 3, userId: 4, amount: 11},
// {id: 4, time: 4, userId: 2, amount: 12},
// ]
// Filters should support operations like "=", ">", "<" etc
// The question is open ended. You need to decide how the input should look like.

/*
5:41 pm
Transaction

Filters: id = 10 or (name = sammer or money < 10)
*/
/*
Types of pagination
1. Offset and limit based
2. Keyset based
3. Cursor based
*/

#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

using namespace std;

struct Transaction {
    int id;
    int time;
    int userId;
    int amount;
    string message;
    friend ostream& operator<<(ostream &stream, const Transaction &t) {
        stream << t.id <<" " << t.time <<" " << t.userId <<" "<< t.amount << " " << t.message << endl;

        return stream;
    }
};

template<typename T>
class BaseFilter {
    public:
    virtual bool matches(const T&) const = 0;
    virtual ~BaseFilter() {}
};


enum class Operator {
    EQ,
    NOT_EQ,
    LESS_EQ,
    LESS,
    GREATER,
    GREATER_EQ
};
// id = 10 or money < 10;
template<typename T, typename FieldType>
class FieldFilter: public BaseFilter<T> {
    using FieldGetter = function<FieldType(const T&)>;
    FieldType value;
    Operator op;
    FieldGetter func;

    public:
    FieldFilter(FieldGetter func, const Operator &op, const FieldType& value):value(value), op(op), func(func) {}

    bool matches(const T& item) const override {
        auto search_value = func(item);
        // cout << search_value <<" " << value << " " << (search_value > value) << endl;
        switch (op) {
            // Instead of switch use a map here and we can have custom functions there instead of these hardcoded ones.
            case Operator::EQ: return search_value == value;
            case Operator::GREATER: return search_value > value;
            case Operator::GREATER_EQ: return search_value >= value;
            case Operator::LESS: return search_value < value;
            case Operator::LESS_EQ: return search_value <= value;
            case Operator::NOT_EQ: return search_value != value;
        }
    } 
};


template<typename T>
class AndFilter : public BaseFilter<T> {
    vector<shared_ptr<BaseFilter<T>>> filters;
    public:
    void addFilter(shared_ptr<BaseFilter<T>> filter) {
        filters.push_back(move(filter));
    }

    bool matches(const T& item) const override {
        for(auto &filter: filters) {
            if (!filter->matches(item)) return false;
        }

        return true;
    }
};

template<typename T>
class OrFilter: public BaseFilter<T> {
    vector<shared_ptr<BaseFilter<T>>> filters;
    public:
    void addFilter(shared_ptr<BaseFilter<T>> filter) {
        filters.push_back(move(filter));
    }

    bool matches(const T& item) const override {
        for(auto &filter: filters) {
            if (filter->matches(item)) return true;
        }

        return false;
    }
};

template<typename T>
class SearchBuilder {
    vector<T> _data;
    shared_ptr<BaseFilter<T>> baseFilter;
    vector<T>::iterator start, end;
    public:
    SearchBuilder(vector<T> data, shared_ptr<BaseFilter<T>> baseFilter):
    _data(data),
    baseFilter(move(baseFilter)),
    start(_data.begin()),
    end(_data.end())
    {}

    bool hasNext() {
        while(start != end and !baseFilter->matches(*start)) start++;

        return start != end;
    }


    const T next() {
        return *(start++);
    }

    vector<T> next(int pageSize) {
        vector<T> results;
        while(start != end and results.size() < pageSize) {
            if (!baseFilter->matches(*start)) continue;
            results.push_back(*start);
            start++;
        }

        return results;
    }

};

int main() {
    vector<Transaction> transactions = {
        {1, 1000, 3, 100, "Hello this is my first transaction"},
        {2, 1200, 4, 10, "Hello this is the second one"},
        {3, 4000, 3, 200, "Third one and I am already tired writing all this"},
        {4, 4100, 2, 1000, "Fourth, now I am just wasting time"},
        {5, 4200, 5, 10, "I don't want to write anymore"},
        {6, 5000, 6, 10000, "Idc"},
        {7, 6000, 6, 2, "You cheapstake"},
    };


    auto idFilter = make_shared<FieldFilter<Transaction, int>>([](const Transaction& t) -> int {
        return t.id;
    }, Operator::GREATER_EQ, 0);

    string value = "You cheapstake";
    auto func = [](const Transaction &t) -> string {
        return t.message;
    };
    auto messageFilter = make_shared<FieldFilter<Transaction, string>>(func, Operator::EQ, value);
    auto andFilter = make_shared<OrFilter<Transaction>>();
    andFilter->addFilter(messageFilter);
    andFilter->addFilter(idFilter);
    SearchBuilder<Transaction> builder(transactions, andFilter);
    while(builder.hasNext()) {
        for(auto &x: builder.next(3)) {
            cout << x << endl;
        }
        cout << endl;
        // cout << builder.next();
        // builder.next();
    }

}
