/*
Here we will work on designing and implementing a complete In-Memory database with indexing using OOD with the following requirements:

The user should be able to define columns using the name and data type and can also specify custom validations on them such as the value cannot be null for string column type, the value should be in provided range for integer column type, column value can be required or not, etc.
The user should be able to create tables using a defined schema, where the schema is a collection of columns with certain validations.
The user should be able to insert/update/delete data in a table. Any errors during schema validations are to be raised to the user.
The user should be able to query data using multiple filters on any of the columns.
The user can create additional indexes on any columns and of any type like reverse, fuzzy, etc and the same should be used for querying.
In one query, multiple filter criteria can be sent and data needs to be filtered accordingly.


// Consider String, int, doubles, floats, long long, long int
Components
1. Schema
- number of columns
- validation condition

2. Row has a Schema
    validate()
12:36 

Validator -> validate(const string&)
Schema = {vector<Validator>, index index of primary key, other indexes + type;
Table table(schema)
table.insert(vector<string>) -> false, true with error message.
table.delete(primary key) -> bool
table.update(vector<string>) -> If doesn't exist return error otherwise validate and update.
BaseFilter
FieldFilter, AndFilter, OrFilter -> A == x or B == y
table.search(BaseFilter f) f.matches(rows) -> return cursor 

Schema(funct1, funct1)
*/
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>

using namespace std;

enum class DataType {
    INT,
    STRING,
    DOUBLE,
    LONG_LONG_INT,
    FLOAT,
};

class Value {
    string value;
    DataType type;
    public:
    Value(const string &&value, const DataType& type): value(value), type(type) {};
    
    optional<int> getInteger() const {
        try {
            return stoi(value);
        } catch (...) {
            return nullopt;
        }
    }

    optional<long long> getLongInteger() const {
        try {
            return stoll(value);
        } catch (...) {
            return nullopt;
        }
    }

    string get_string() const {
        return value;
    }

    optional<double> get_double() const {
        try {
            return stod(value);
        } catch(...) {
            return nullopt;
        }
    }

    DataType getType() const {
        return type;
    }

    bool operator ==(const Value& other) const {
        if (other.type != type) return false;
        return other.get_string() == get_string();
    }
};

struct Hash {
    std::size_t operator()(const Value& v) const {
        return hash<string>()(v.get_string());
    }
};

class Validator {
    using ValidateFunc = function<bool(const Value&)>; 
    ValidateFunc validator;
    public:
    Validator(const ValidateFunc validator):validator(validator) {}
    bool validate(const Value& v, const DataType& type) {
        return v.getType() == type and validator(v);
    }
};

enum class SecondaryIndexTypes {
    INVERTED,
    SORTED,
    HASH
};

class Row {
    public:
    vector<Value> values;
    Row(vector<Value>& values): values(values) {}
};
class Schema {
    int columnNum;
    vector<DataType> types;
    unordered_map<int, shared_ptr<Validator>> validators;
    int primaryKeyIndex;
    vector<pair<int, SecondaryIndexTypes>> secondaryIndexes;
    public:
    Schema(int columns, const vector<DataType>& types, unordered_map<int, shared_ptr<Validator>> &validators, int primaryKeyIndex = 0):
    columnNum(columns),
    types(types),
    validators(move(validators)),
    primaryKeyIndex(primaryKeyIndex),
    secondaryIndexes(secondaryIndexes)

    {}

    int getPrimaryKeyIndex() {return primaryKeyIndex;}
    bool validate(const unique_ptr<Row> & row) {
        if (columnNum != types.size())  return false;
        for(auto &[index, validator]: validators) {
            if (index >= columnNum) return false;
            if (!validator->validate(row->values[index], types[index])) return false; 
        }

        return true;
    }
};

class Table {
    shared_ptr<Schema> schema;
    vector<unique_ptr<Row>> rows;
    unordered_map<Value, int, Hash> primaryIndex;
    int numOfRows;
    public:
    Table(shared_ptr<Schema> schema):schema(std::move(schema)), numOfRows(0) {}
    pair<int, string> insert(vector<Value> &values) {
        auto row = make_unique<Row>(values);
        if (!schema->validate(row)) {
            return {-1, "Wrong types provided"};
        }
        int index = rows.size();
        rows.emplace_back(std::move(row));
        primaryIndex[row->values[schema->getPrimaryKeyIndex()]] = index;
        return {0, "Inserted succesfully"};
    }

    bool remove(const Value& value) {
        // Assuming the value is primary key
        if (primaryIndex.find(value) == primaryIndex.end()) return false;
        primaryIndex.erase(value);
        return true;
    }

    bool update(vector<Value> &values) {
        const auto &value = values[schema->getPrimaryKeyIndex()];
        if (primaryIndex.find(value) == primaryIndex.end()) return false;
        int index = primaryIndex[value];
        auto &row = rows[index];
        row->values = values;
        return true;
    }

    void printRows() {
        for(auto &row : rows) {
            for(auto &value: row->values) cout << value.get_string() <<" ";
            cout << endl; 
        }
    }
};




int main () {
    auto v1 = make_shared<Validator>([](const Value& value) -> bool {
        if (value.get_string().size() == 0) return false;
        cout << value.get_string() << endl;
        return value.get_string() > "a" and value.get_string() < "y";
    });

    auto v2 = make_shared<Validator>([](const Value& value) -> bool {
        cout << value.get_string() << endl;
        return value.getInteger() >= 10;
    });

    auto v3 = make_shared<Validator>([](const Value& value) -> bool {
        cout << value.get_string() << endl;
        return value.get_double() >= 4.5;
    });

    unordered_map<int, shared_ptr<Validator>> validators = {
        {0, std::move(v1)},
        {1, std::move(v2)},
        {2, std::move(v3)}
    } ;
    auto schema = make_shared<Schema>(3, vector<DataType>({DataType::STRING, DataType::INT, DataType::DOUBLE}), validators);
    Table t(schema);
    vector<Value> values = vector<Value>({ Value("Ritwiz", DataType::STRING), Value("100", DataType::INT), Value("1866.5", DataType::DOUBLE)});
    auto [result, message] = t.insert(values);
    cout << message << endl;
    t.printRows();
}
