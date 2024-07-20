/*
 * Description: The building class is the core of the program. It contains all the elevators and floors, and it will
 * refresh every second. The refresh function `run()` will spawn passengers, update elevators and floors, and update the statistics.
 */

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
    building();  // read configuration data, create elevators and floors

    void set_monitor(monitor *_monitor);  // set monitor

    // Methods
    void run();  // refresh the building

    void remove_passenger(passenger *p);  // remove passenger from building

    [[nodiscard]] nlohmann::json get_conf() const;  // get configuration data

private:
    static std::random_device rd;  // obtain a random number from hardware
    static std::mt19937 e;  // random number generator
    std::poisson_distribution<int> rand_passenger_normal;  // generate random number of passengers
    std::poisson_distribution<int> rand_passenger_peak;  // generate random number of passengers

    monitor *mon = nullptr;  // monitor for the building
    nlohmann::json conf;  // store configuration data
    long long refresh_time_stamp;  // time stamp of the last refresh
    long long base_time_stamp;  // time stamp of the first refresh
    int tot_traffic = 0;  // total traffic

    std::vector<class floor *> floors;  // floors in the building
    std::vector<elevator *> elevators;  // elevators in the building
    std::vector<passenger *> passengers;  // passengers in the building
    std::vector<std::pair<int, int>> rush_hours;  // [start, end] of rush hours

    void set_refresh_time();  // set refresh time, invoke after each refresh

    [[nodiscard]] long long get_time_gap() const;  // get time gap between now and last refresh

    [[nodiscard]] bool is_rush_hour() const;  // check if it is rush hour
};

#endif //BUILDING_H
