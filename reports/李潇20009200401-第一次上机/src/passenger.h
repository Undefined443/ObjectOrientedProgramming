#ifndef PASSENGER_H
#define PASSENGER_H

#include "elevator.h"
#include "floor.h"
#include "building.h"
#include "monitor.h"
#include "nlohmann/json.hpp"
#include <random>

class monitor;

class passenger {
public:
    passenger(class building *b, class floor *f, const nlohmann::json &conf);

    void board(elevator *el);  // set current elevator and clear current floor
    void alight(class floor *f);  // set current floor and clear current elevator and deactivate the passenger
    void leave_building();
    class floor *get_current_floor();
    int get_destination() const;
    void run();

    void set_monitor(monitor *m);
private:
    static std::random_device rd;  // obtain a random number from hardware
    static std::mt19937 e;  // random number generator

    const nlohmann::json conf;

    std::uniform_int_distribution<int> rand_boarding_time;  // generate random boarding time
    std::uniform_int_distribution<int> rand_floor;  // generate random floor destination
    std::uniform_int_distribution<int> rand_active_time;  // how long a passenger require an elevator

    int original_floor = 1;
    int destination_floor;
    int boarding_time;  // random boarding time for each passenger
    int active_time;  // random active time for each passenger
    bool activated = false;  // whether the passenger is active
    class floor *current_floor;
    elevator *current_elevator = nullptr;
    building *current_building;
    void set_random_dest();
    monitor *mon;

    long long base_time_stamp;  // base time stamp for elevator

    void set_base_time();  // set base time for elevator
    long long get_time_gap() const;  // calculate time gap between current time and last run time
};


#endif //PASSENGER_H
