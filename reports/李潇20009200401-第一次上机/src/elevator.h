#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "passenger.h"
#include "monitor.h"
#include "nlohmann/json.hpp"
#include <vector>
#include <queue>
#include <map>

class passenger;
class floor;
class monitor;

class elevator {
public:
    elevator(monitor *mon, int id, const nlohmann::json& conf);
    void set_floors(std::vector<class floor*> &fv);  // set floors accessible by the elevator
    void reg_pas(passenger *p);  // if a passenger want to board the elevator, register it (by floor)

    void ding();  // the elevator has arrived at a floor

    void run();  // move elevator up or down

    // getters
    class floor *get_current_floor();
    int get_direction() const;

    enum direction {up = 1, down = -1, stop = 0};
private:
    int id;
    monitor *mon;
    class floor *current_floor;
    nlohmann::json conf;  // configuration data
    int direction = 0;  // 1 = up, -1 = down, 0 = stopped
    bool status = false;  // true = running, false = idle

    std::vector<class floor*> floors;  // floors attached to the elevator
    std::vector<passenger*> passengers;  // passengers on the elevator
    std::map<class floor*, std::vector<passenger*>> registry;  // table of passengers to be boarded/alighted at each floor

    long long refresh_time_stamp;  // base time stamp for elevator

    void board();  // board passengers
    void alight();  // alight passengers
    int choose_direction();  // choose the direction of the elevator
    void set_base_time();  // set base time for elevator
    long long get_time_gap() const;  // calculate time gap between current time and last run time
};


#endif //ELEVATOR_H
