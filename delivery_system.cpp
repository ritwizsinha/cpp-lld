/*
Given a list of dishes and its cost in the format < restaurant_id, dishname, dish cost> and the position of restaurant <restaurant_id, x coordinate y coordinate> and the position of user and average speed of delivery executive.

Design a food delivery app which when given a dishname, provides us the fastest and cheapest restaurant
Now we have a stream of orders, find different analytics like number of orders in a given timeframe, average order cost in a time frame and so on


Entity:
1. Dish: name, cost, restaurant
2. Restaurants: list<dishes, cost>, location
3. Locations: x, y
4. Users: name, Location
5. OrderManager (time, dishname, cost)
    ----> OrderAnalytics
*/
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

using namespace std;

struct Dish
{
    string name;
    bool operator==(const Dish& other) const {
        return other.name == name;
    }

    Dish(const string &name) : name(name) {}
};

template<>
struct hash<Dish> {
    size_t operator()(const Dish& d) const {
        return hash<string>()(d.name);
    }
};

struct Coordinates {
    int x;
    int y;
    Coordinates(int x, int y): x(x), y(y) {}
    int distance(const Coordinates& location) {
        return abs(location.x - x) + abs(location.y - y);
    }

    Coordinates() {};
};

struct Restaurant {
    Coordinates location;
    size_t id;
    Restaurant(int id, const Coordinates &location):id(id), location(location) {};
    Restaurant(int id):id(id) {};
    unordered_map<Dish, unsigned int> costs;

    unsigned int findCost(const Dish& d) {
        if (costs.find(d) == costs.end()) return -1;
        return costs[d];
    }

    void addDish(const Dish& d, int cost) {
        costs[d] = cost;
    }

    bool hasDish(const Dish& d) {
        return costs.find(d) != costs.end();
    }
};

struct User {
    Coordinates location;
    unsigned int id;
    User(const Coordinates& location, unsigned int id): location(location), id(id) {}
};


struct InventoryItem {
    unsigned int id;
    Dish dish;
    unsigned int cost;

    InventoryItem(unsigned int id, const Dish& dish, unsigned int cost): id(id), dish(dish), cost(cost) {}
};

struct Position {
    unsigned int id;
    Coordinates location;
    Position(unsigned int id, const Coordinates& location):id(id), location(location) {}
};

struct Order {
    unsigned long long timestamp;
    unsigned int cost;
    Dish dish;
    User user;

    Order(unsigned long long timestamp, unsigned int cost, const Dish &dish, const User &user):
    timestamp(timestamp),
    cost(cost),
    dish(dish),
    user(user)
    {

    }

};

class FindStrategy {
    public:
    virtual size_t find_restaurant(const Order& order) = 0;
    virtual ~FindStrategy() { }

};


class RestaurantManager {
    private:
    RestaurantManager() {};
    public:
    static once_flag init;
    static unique_ptr<RestaurantManager> instance;
    static unique_ptr<FindStrategy> strategy;
    static unordered_map<unsigned int, unique_ptr<Restaurant>> restaurants;
    RestaurantManager(const RestaurantManager&) = delete;
    RestaurantManager& operator=(const RestaurantManager&) = delete;
    static vector<function<void(const Order&)>> subscribers;


    static RestaurantManager* get_instance() {
        call_once(init, []() {
            instance.reset(new RestaurantManager());
        });

        return instance.get();
    }

    static void setFindStrategy(FindStrategy* strat) {
       strategy.reset(strat); 
    }

    static void initializeApp(vector<InventoryItem> &inventory) {
        for(auto [id, dish, cost] : inventory) {
            if (restaurants.find(id) == restaurants.end()) {
                restaurants[id] = make_unique<Restaurant>(id);
            }

            auto restaurant = restaurants[id].get();
            restaurant->addDish(dish, cost);
        }
    }

    static void initializeLocations(vector<Position> &locations) {
        for(auto [id, location]: locations) {
            if (restaurants.find(id) == restaurants.end()) {
                restaurants[id] = make_unique<Restaurant>(id);
            }

            auto restaurant = restaurants[id].get();
            restaurant->location = location;
        }
    }

    static void makeOrder(const User& user, const Dish& dish, unsigned long long timestamp) {
        Order order(timestamp, 0, dish, user);
        auto idx = strategy.get()->find_restaurant(order);
        order.cost = restaurants[idx].get()->findCost(dish);
        notifySubscribers(order);
        if (idx == -1) {
            cout << "Item not found" << endl;
        }  else {
            cout << idx <<" is cooking the dish " << dish.name << endl;
        }
    }

    static void notifySubscribers(const Order& order) {
        for(auto &s : subscribers) {
            s(order);
        }
    }
};


once_flag RestaurantManager::init;
unique_ptr<RestaurantManager> RestaurantManager::instance = nullptr;
unique_ptr<FindStrategy> RestaurantManager::strategy = nullptr;
unordered_map<unsigned int, unique_ptr<Restaurant>> RestaurantManager::restaurants;
vector<function<void(const Order&)>> RestaurantManager::subscribers;

class FindClosestStrategy : public FindStrategy {
    public:
    RestaurantManager *manager;
    FindClosestStrategy(RestaurantManager* manager): manager(manager) {}
    size_t find_restaurant(const Order& order) {
        unsigned int id = -1;
        unsigned int distance = UINT_MAX;
        for(auto itr = manager->restaurants.begin(); itr != manager->restaurants.end(); itr++) {
            auto restaurant = itr->second.get();
            if (restaurant->hasDish(order.dish)) {
                if (distance > restaurant->location.distance(order.user.location)) {
                    distance = restaurant->location.distance(order.user.location);
                    id = restaurant->id;
                }
            }
        }

        return id;
    }
};

class FindCheapestStrategy : public FindStrategy {
    public:
    RestaurantManager *manager;
    FindCheapestStrategy(RestaurantManager* manager): manager(manager) {};
    size_t find_restaurant(const Order& order) {
        unsigned int id = -1;
        unsigned int cost = UINT_MAX;
        for(auto itr = manager->restaurants.begin(); itr != manager->restaurants.end(); itr++) {
            auto restaurant = itr->second.get();
            if (restaurant->hasDish(order.dish)) {
                if (cost > restaurant->findCost(order.dish)) {
                    cost = restaurant->findCost(order.dish);
                    id = restaurant->id;
                }
            }
        }

        return id;
    }
};




class OrderAnalytics {
    public:
    static vector<Order> orders;
    static vector<unsigned long long> costsSum;

    static void pushOrder(const Order& order) {
        orders.push_back(order);
        if (costsSum.size() == 0) costsSum.push_back(order.cost);
        else costsSum.push_back(costsSum.back() + order.cost);
    }

    static int findOrdersInRange(unsigned long long start, unsigned long long end) {
        int l = findIndex(start);
        int r = findIndex(end);
        if (r == -1) return 0;
        return l == -1 ? r + 1 : r - l;
    }


    static int findOrderCostInRange(unsigned long long start, unsigned long long end) {
        // for(auto &x : costsSum ) cout << x <<" " ;
        // cout <<endl;
        int l = findIndex(start);
        int r = findIndex(end);
        if (r == -1) return 0;
        return l == -1 ? costsSum[r] : costsSum[r] - costsSum[l];
    }

    


    private:
    static int findIndex(unsigned long long &timestamp) {
        int lo = 0, hi = orders.size() - 1;
        while(lo < hi) {
            int mid = lo + (hi - lo + 1) / 2;
            if (orders[mid].timestamp <= timestamp) lo = mid;
            else hi = mid - 1;
        }

        return orders[lo].timestamp <= timestamp ? lo : -1;
    }
    
};

vector<unsigned long long> OrderAnalytics::costsSum;
vector<Order> OrderAnalytics::orders;


int main() {
    /*
    Dish: Burger, Dosa, Idli, Pizza, Biryani, Rajma Chawal, Pasta, Wine, Sandwich, Thali

    Restaurants: A 
    
    
    */
   vector<InventoryItem> l = {
    InventoryItem(1313, Dish("Burger"), 400),
    InventoryItem(1200, Dish("Dosa"), 420),
    InventoryItem(1000, Dish("Pizza"), 100),
    InventoryItem(1200, Dish("Pizza"), 200),
    InventoryItem(1400, Dish("Rajma Chawal"), 200),
    InventoryItem(1313, Dish("Rajma Chawal"), 120),
    InventoryItem(1000, Dish("Rajma Chawal"), 140)
   };

   vector<Position> p = {
    Position(1313, Coordinates(10, 10)),
    Position(1200, Coordinates(6, 5)),
    Position(1000, Coordinates(8, 9)),
    Position(1400, Coordinates(1,2))
   };

   auto manager = RestaurantManager::get_instance();
   manager->subscribers.push_back(OrderAnalytics::pushOrder);
   manager->initializeApp(l);
   manager->initializeLocations(p);

   auto closestStrategy = new FindClosestStrategy(manager);
   auto cheapestStrategy = new FindCheapestStrategy(manager);
   manager->setFindStrategy(closestStrategy);
   User user(Coordinates(0, 0), 1);
   manager->makeOrder(user, Dish("Rajma Chawal"), 100);

   manager->setFindStrategy(cheapestStrategy);
   manager->makeOrder(user, Dish("Rajma Chawal"), 400);
   manager->makeOrder(user, Dish("Pizza"), 500);

   cout << OrderAnalytics::findOrderCostInRange(0, 500) << endl;
   cout << OrderAnalytics::findOrdersInRange(0, 500) << endl;
   cout << OrderAnalytics::findOrderCostInRange(0, 100) << endl;
   cout << OrderAnalytics::findOrdersInRange(0, 100) << endl;
   cout << OrderAnalytics::findOrderCostInRange(101, 500) << endl;
   cout << OrderAnalytics::findOrdersInRange(101, 500) << endl;

}