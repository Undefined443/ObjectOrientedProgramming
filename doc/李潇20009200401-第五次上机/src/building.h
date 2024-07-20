#ifndef BUILDING_H
#define BUILDING_H

#include "elevator.h"
#include "floor.h"
#include "passenger.h"
#include "monitor.h"
#include "nlohmann/json.hpp"
#include <vector>
#include <random>
#include <chrono>

class passenger;
class floor;
class elevator;
class monitor;

class building {
public:
    friend class monitor;

    // Setup
    building();

    void set_monitor(monitor *_monitor);

    // Methods
    void run();  // refresh the building

    void remove_passenger(passenger *p);  // remove passenger from building

    nlohmann::json get_conf() const;  // get configuration data

private:
    static std::random_device rd;  // obtain a random number from hardware
    static std::mt19937 e;  // random number generator
    std::poisson_distribution<int> rand_passenger_normal;  // generate random number of passengers
    std::poisson_distribution<int> rand_passenger_peak;  // generate random number of passengers

    monitor *mon = nullptr;  // monitor for the building
    nlohmann::json conf;  // store configuration data
    long long refresh_time_stamp;
    long long base_time_stamp;
    int tot_traffic = 0;

    std::vector<class floor *> floors;  // floors in the building
    std::vector<elevator *> elevators;  // elevators in the building
    std::vector<passenger *> passengers;  // passengers in the building
    std::vector<std::pair<int, int>> rush_hours;  // [start, end] of rush hours

    void set_refresh_time();  // set refresh time, invoke after each refresh

    long long get_time_gap() const;

    bool is_rush_hour() const;
};


#endif //BUILDING_H
