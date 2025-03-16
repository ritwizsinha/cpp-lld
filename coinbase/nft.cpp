/*

- Part 1: You're given a list of values of object property.
    - Example: The object is a tree.
    - Length:[ Short, medium, tall]
    - Leaf colour: [Green, yellow, red]
    - Like this, you have multiple properties.
    - Now create a list of random objects by randomly selecting each property's value.
- Part 2: Generate unique objects in part 1.
- Part 3: Add a rarity option to the properties. For example, if the 'Tall' property is 'rare' then there should be only a few tall trees when you create random trees in part 2.

Property
Property(Vector<T>)
getValue

Object
- vector<Property>
- addProperty
- getObject
12:00 pm 
*/

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <unordered_set>
#include <random>

using namespace std;

template<typename T>
class Property {
    vector<T> _elements;
    vector<double> weights;
    public:
    Property(const vector<T> &elements, const vector<double> &weights) : _elements(std::move(elements)), weights(weights) {
    }
    const T getValue() {
        random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<int> dist(begin(weights), end(weights));
        return _elements[dist(gen)];
    } 
};

template<typename T>
struct NFT {
    vector<T> properties;
};

template<typename T>
concept StringConvertible = requires(T t) {
    {std::to_string(t)} -> std::convertible_to<std::string>;
} || std::is_convertible_v<T, std::string> ;


template<typename T>
concept isString = std::is_same_v<std::decay_t<T>, std::string>;


template<StringConvertible PropertyType>
class NFTBuilder {
    private:
    static vector<Property<PropertyType>> collection;
    static unordered_set<string> _store;

    public:
    static vector<Property<PropertyType>>& collect() {
        return collection;
    }

    static vector<PropertyType> getNft() {
        vector<PropertyType> results;
        string key;
        int iterations = 0;
        do {
            results.clear();
            for(auto &coll : collection) {
                results.push_back(coll.getValue());
            } 

            key = updateStore(results);
            // cout << "Showing Iterations: " << key << " " << iterations++ << endl;
            iterations++;
        } while(_store.find(key) != _store.end() and iterations < 100);


        
        _store.insert(key);
        return results;
    }

    static string updateStore(vector<PropertyType> &nft) {
        string key;
        for(auto &x : nft) {
            if constexpr (isString<PropertyType>)
                key.append(x);
            else if constexpr (StringConvertible<PropertyType>)
                key.append(to_string(x));
        }
        return key;
    }

};

template<StringConvertible T>
vector<Property<T>> NFTBuilder<T>::collection;

template<StringConvertible T>
unordered_set<string> NFTBuilder<T>::_store;

int main() {
    srand(time(0));
    vector<string> height = {"tall", "short", "long", "medium", "high", "micro", "very small"};
    vector<string> color = {"red", "yellow", "blue", "green", "orange", "white", "beige"};
    vector<string> age = {"some years", "very old", "prehistoric", "daddy of all trees", "very very old", "just born", "dead"};
    vector<double> weights = {0.6, 0.1, 0.1, 0.1, 0.05, 0.025, 0.025};

    Property<string> heightProp(height, weights);
    Property<string> colorProp(color, weights);
    Property<string> ageProp(age, weights);

    NFTBuilder<string>::collect() = {heightProp, colorProp, ageProp};

    for(int i = 0; i < 10; i++) {
        for(auto &x: NFTBuilder<string>::getNft()) 
            cout << x <<" ";

        cout << endl;
    }

}


