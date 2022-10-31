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
    friend class monitor;

    elevator(int id, const nlohmann::json& conf);

    void set_floors(std::vector<class floor *> &fv);  // set floors accessible by the elevator

    void set_monitor(monitor *m);  // set the monitor

    void reg_pas(passenger *p);  // if a passenger want to board the elevator, register it (by floor)

    void ding();  // the elevator has arrived at a floor

    void run();  // move elevator up or down

    // getters
    class floor *get_current_floor();

    int get_direction() const;

    bool get_status() const;

    int get_ding_stage() const;

    bool is_full() const;

    int get_alighting_num(class floor *f);

    enum direction {
        up = 1, down = -1, stop = 0
    };

    enum status {
        running = 1, idle = 0
    };

private:
    int id;
    monitor *mon;
    nlohmann::json conf;  // configuration data
    long long refresh_time_stamp;  // base time stamp for elevator

    int direction = stop;
    bool status = idle;
    bool full = false;
    int ding_stage = 0;  // indicates at which stage the ding function is running

    class floor *current_floor;
    std::vector<class floor *> floors;  // floors attached to the elevator
    std::vector<passenger *> passengers;  // passengers on the elevator

    std::map<class floor *, std::vector<passenger *>> registry;  // table of passengers to be boarded/alighted at each floor

    void board();  // board passengers

    void alight();  // alight passengers

    int choose_direction();  // choose the direction of the elevator

    void set_refresh_time();  // set base time for elevator

    long long get_time_gap() const;  // calculate time gap between current time and last run time
};


#endif //ELEVATOR_H
