/*
Requirements
1. The parking lot should have multiple levels, each level with a certain number of parking spots.
2. The parking lot should support different types of vehicles, such as cars, motorcycles, and trucks.
3. Each parking spot should be able to accommodate a specific type of vehicle.
4. The system should assign a parking spot to a vehicle upon entry and release it when the vehicle exits.
5. The system should track the availability of parking spots and provide real-time information to customers.
6. The system should handle multiple entry and exit points and support concurrent access.

Entities
ParkingLot, Vehicle, Levels, FloorManager, ParkingManager, ParkingSpot

VehicleType
- type string

Vehicle
- VehicleType
- id

ParkingSpot
- VehicleType
- occupied
- occupied_at
- spotId
- vehicleId


FloorManager
- registerEntry(id)
- registerExit(id)
- List<ParkingSpot>  freepool
- Map<Id> ParkingSpot

ParkingManager
- List<FloorManager> levels
- FillingStrategy




*/

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <mutex>

using namespace std;

enum class VehicleType {
    TWO_WHEELER,
    FOUR_WHEELER,
    HEAVY,
};

class Vehicle {
    public:
    string id;
    VehicleType type;
    Vehicle(const string&id, const VehicleType &type) : id(id), type(type) {}
};

class ParkingSpot {
    public:
    string spot_id;
    bool occupied;
    int occupied_at;
    VehicleType type;
    ParkingSpot(const string& spot_id, const VehicleType& type) : spot_id(spot_id), occupied(false), type(type) {}
};

class FCFSstrategy : public FillingStrategy {
    public:
    unordered_map<string, ParkingSpot> &emptyPool;
    FCFSstrategy(unordered_map<string, ParkingSpot> &emptyPool): emptyPool(emptyPool) {};
    string operator()() override {
        if (emptyPool.empty()) return "";
        return emptyPool.begin()->first;
    }
};

class FillingStrategy {
    public:
    virtual string operator()() = 0;
};


class LevelStrategy {
    public:
    virtual int operator()() = 0;
};

class RandomLevels : public LevelStrategy {
    int floors;
    RandomLevels(int floors): floors(floors) {};
    int operator()() override {
        return rand() % floors;
    }
};

class FloorManager {
    public:
    FloorManager(string floor_id, unique_ptr<FillingStrategy> &strategy, vector<ParkingSpot> spots):floor_id(floor_id), strategy(move(strategy)),parking_spots(begin(spots), end(spots)) {
    }
    unique_ptr<FillingStrategy> strategy;
    string floor_id;
    unordered_map<string, ParkingSpot> locations;
    unordered_map<string,ParkingSpot> parking_spots;
    bool register_entry(const string &id) {
        if (locations.find(id) != locations.end())  return false;
        if (parking_spots.size() == 0) return false;

        string id = (*strategy.get())();
        auto spot = parking_spots[id];
        spot.occupied = true;
        spot.occupied_at = time(0);
        locations[id] = spot;
        parking_spots.erase(id);

        return true;
    }

    bool register_exit(const string &id) {
        if (locations.find(id) == locations.end()) return false;

        auto spot = locations[id];
        spot.occupied = false;
        spot.occupied_at = 0;
        parking_spots[id] = spot;
        locations.erase(id);

        return true;
    }


    optional<ParkingSpot> get_parking_spot(const string &id) {
        if (locations.find(id) == locations.end()) return nullopt;
        return locations[id];
    }
};

class FareCalculator {
    public:
    virtual int calculateFare(int in_time, int out_time) = 0; 
};

class TwoWheelerFareCalculator : public FareCalculator {
    public:
    int in_time, out_time;
    TwoWheelerFareCalculator(){}

    int calculateFare(int in_time, int out_time) override {
        return (out_time - in_time) * 100;
    }
};

class FourWheelerFareCalculator : public FareCalculator {
    public:
    int in_time, out_time;
    FourWheelerFareCalculator() {}

    int calculateFare(int in_time, int out_time) override {
        return (out_time - in_time) * 400;
    }
};

class HeavyFareCalculator : public FareCalculator {
    public:
    int in_time, out_time;
    HeavyFareCalculator() {}

    int calculateFare(int in_time, int out_time) override {
        return (out_time - in_time) * 800;
    }
};




class ParkingLot {
    vector<FloorManager> levels;
    unique_ptr<LevelStrategy> strategy;
    unordered_map<string, int> levelMapping;
    mutex m;
    
    bool addVehicle(const string &id) {
        int level = (*strategy.get())();

        unique_lock lock(m);
        auto possible = levels[level].register_entry(id);
        lock.unlock();
        if (possible) {
            levelMapping[id] = level;
        }

        return possible;
    }

    bool removeVehicle(const string &id) {
        if (levelMapping.find(id) == levelMapping.end()) return false;
        lock_guard lock(m);
        return levels[levelMapping[id]].register_exit(id);
    }

    int calculateFare(const string &id) {
        if (levelMapping.find(id) == levelMapping.end()) return -1;
        auto level = levelMapping[id];
        auto spot = levels[level].get_parking_spot(id);
        if (!spot.has_value()) return -1;
        auto type = spot.value().type;
        auto starting_time = spot.value().occupied_at;
        auto fareCalculator = fareMapper[type].get();
        return (*fareCalculator).calculateFare(starting_time, time(0));
    }

};


unordered_map<VehicleType, unique_ptr<FareCalculator>> fareMapper;



/*
Prompt:
Here is the design for the problem statement described in the starting in comments. This is a low level design problem.
I have implemented the requirements provided in the starting. Verify like an interviewer how accurate my assumptions and implementations are.alignas
Take into account all of the design patterns are possible in cpp
1. Give suggestions for improvements
2. Figure out what was did well here
3. Find edge cases in the implementation
4. What other design patterns could be used here.

*/