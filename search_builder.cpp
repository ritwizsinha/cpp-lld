/*
You will be given a list of transactions. You need to write a generic search api to filter on a combination of specific fields and values
Example :
transactions = [
{id: 1, time: 1, userId: 1, amount: 10},
{id: 2, time: 2, userId: 3, amount: 10},
{id: 3, time: 3, userId: 4, amount: 11},
{id: 4, time: 4, userId: 2, amount: 12},
]
Filters should support operations like "=", ">", "<" etc
The question is open ended. You need to decide how the input should look like.


Follow up :
Explain pagination and why we need it?
What are some of the techniques used for pagination?
How do you choose a cursor id column when using cursor based pagination?
Enhance your code to support cursor based pagination.


Transaction, Search, Filters.
*/

#include <iostream>
#include <functional>
#include <memory>

using namespace std;
enum class Operator {
    EQ,
    NEQ,
    LESS,
    LESS_EQ,
    GREATER,
    GREATER_EQ
};


struct Transaction {
    int id;
    int timestamp;
    int userId;
    int amount;
    int fill;
};

template<typename T>
class BaseFilter {
    public:
    virtual bool matches(const T& t) = 0;
    virtual ~BaseFilter() {}
};

template<typename T, typename FieldType>
class FieldFilter : public BaseFilter<T> {
    private:
    using Getter =  function<FieldType(const T&)>;
    Getter getField;
    Operator op;
    FieldType value;
    public:
    FieldFilter(Getter getField, Operator op, FieldType value):getField(getField),op(op),value(value) {

    }
    bool matches(const T& t) {
        FieldType input = getField(t);
        switch(op) {
            case Operator::EQ: return input == value;
            case Operator::NEQ: return input != value;
            case Operator::LESS: return input < value;
            case Operator::LESS_EQ: return input <= value;
            case Operator::GREATER: return input > value;
            case Operator::GREATER_EQ: return input >= value;
        }

        return false;
    } 
};

template<typename T>
class AndCondition: public BaseFilter<T> {
    private:
    vector<shared_ptr<BaseFilter<T>>> conditions;
    public:
    void addCondition(shared_ptr<BaseFilter<T>> condition)
    {
        conditions.push_back(condition);
    }

    bool matches(const T& t) {
        for(auto condition: conditions) {
            if (!condition->matches(t)) return false;
        }

        return true;
    }
};

template <typename T>
class OrCondition: public BaseFilter<T> {
    private:
    vector<shared_ptr<BaseFilter<T>>> conditions;
    public:
    void addCondition(shared_ptr<BaseFilter<T>> condition)
    {
        conditions.push_back(condition);
    }

    bool matches(const T& t) {
        for(auto condition: conditions) {
            if (condition->matches(t)) return true;
        }
        return false;

    }
};

template<typename T>
class QueryBuilder {
    private:
    shared_ptr<BaseFilter<T>> filter;
    int cursor = 0;

    public:
    QueryBuilder& setCursor(int cursor) {
        this->cursor = cursor;
        return *this;
    }

    QueryBuilder& setFilter(shared_ptr<BaseFilter<T>> condition) {
        this->filter = condition;
        return *this;
    }

    vector<T> execute(const vector<T> &data, int pageSize, int &nextCursor, function<int(const T&)> cursorField) {
        vector<T> result;
        nextCursor = -1;
        for(auto &item : data) {
            if (cursorField(item) <= cursor) continue;
            if (filter and !filter->matches(item)) continue;

            result.push_back(item);
            if (result.size() == pageSize) {
                nextCursor = cursorField(item);
                cursor = nextCursor;
                break;
            }
        }

        return result;
    }
};

int main() {
    vector<Transaction> transactions = {
        {1, 5, 1, 10}, {2, 8, 3, 10}, {3, 15, 4, 11}, {4, 9, 2, 12},
        {5, 6, 5, 15}, {6, 11, 6, 20}, {7, 12, 7, 25}, {8, 13, 8, 30}
    };
// Support queries like (amount > 10 and (userId < 4 or time > 3))
    auto amountCondition = make_shared<FieldFilter<Transaction, int>>([](const Transaction &t) -> int {
        return t.amount;
    }, Operator::GREATER, 11);

    auto userCondition = make_shared<FieldFilter<Transaction, int>>([](const Transaction &t) -> int {
        return t.userId;
    }, Operator::LESS, 0);

    auto timeCondition = make_shared<FieldFilter<Transaction, int>>([](const Transaction &t) -> int {
        return t.timestamp;
    }, Operator::GREATER, 10);

    auto orCondition = make_shared<OrCondition<Transaction>>();
    orCondition->addCondition(timeCondition);
    orCondition->addCondition(userCondition);
    auto andCondition = make_shared<AndCondition<Transaction>>();
    andCondition->addCondition(amountCondition);
    andCondition->addCondition(orCondition);

    
    QueryBuilder<Transaction> builder;


    int cursor = 0;
    int pageSize = 3;
    int nextCursor;


    builder.setCursor(cursor)
    .setFilter(andCondition);

    while(true) {
        auto current_page = builder.execute(transactions, pageSize, nextCursor, [](const Transaction &t) -> int {
            return t.id;
        });

        if (current_page.empty()) break;

        cout << "Page Results:\n";
        for (const auto& txn : current_page) {
            cout << "ID: " << txn.id << ", Time: " << txn.timestamp << ", UserID: " << txn.userId << ", Amount: " << txn.amount << "\n";
        }   

        if (nextCursor == -1) break;
        cout << "Next Cursor: " << nextCursor << "\n\n";

        cursor = nextCursor;
    }
}