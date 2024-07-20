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
    building();
    void set_monitor(monitor *m);
    void run();  // refresh the building
    void remove_passenger(passenger &p);  // remove passenger from building
private:
    static std::random_device rd;  // obtain a random number from hardware
    static std::mt19937 e;  // random number generator

    std::poisson_distribution<int> rand_passenger;  // generate random number of passengers
    nlohmann::json conf;  // store configuration data

    std::vector<class floor*> floors;  // floors in the building
    std::vector<elevator*> elevators;  // elevators in the building
    std::vector<passenger*> passengers;  // passengers in the building
    monitor *mon;  // monitor for the building

    long long refresh_time_stamp;

    unsigned tot_traffic = 0;

    void set_base_time();
    long long get_time_gap() const;
};


#endif //BUILDING_H
