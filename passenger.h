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

    void set_monitor(monitor *m);

    void run();

    bool timer();  // passenger needs some time to alight/board the elevator, if time is up, then passenger can alight/board the elevator

    void board(elevator *el);  // set current elevator and clear current floor

    void alight(class floor *f);  // set current floor and clear current elevator and deactivate the passenger

    class floor *get_current_floor();

    int get_destination() const;

    int get_id() const;

private:
    static int c_id;
    static std::random_device rd;  // obtain a random number from hardware
    static std::mt19937 e;  // random number generator

    std::uniform_int_distribution<int> rand_boarding_time;  // generate random boarding time
    std::uniform_int_distribution<int> rand_floor;  // generate random floor destination
    std::uniform_int_distribution<int> rand_active_time;  // how long a passenger require an elevator
    std::uniform_int_distribution<int> rand_total_destination;  // how many floors a passenger will visit

    const nlohmann::json conf;
    monitor *mon;

    long long refresh_time_stamp;  // refresh time stamp for passenger
    long long timer_time_stamp;  // time stamp for timer

    int id;
    int original_floor;
    int destination_floor;
    int count_destinations = 0;  // count how many floors a passenger has visited
    int total_destinations;
    class floor *current_floor;
    int boarding_time;  // random boarding time for each passenger
    int active_time;  // random active time for each passenger
    bool activated = false;  // whether the passenger is active
    bool is_timing = false;  // whether the timer is running

    elevator *current_elevator = nullptr;
    building *current_building;

    void set_random_dest();

    void set_refresh_time();  // set base time for elevator

    long long get_time_gap() const;  // calculate time gap between current time and last run time
};


#endif //PASSENGER_H
