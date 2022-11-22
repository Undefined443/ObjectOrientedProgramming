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

    void set_current_floor(class floor *f);  // set initial floor

    void add_accessible_floor(class floor *f);  // set floors accessible by the elevator

    void set_floors(std::vector<class floor *> &floors);  // set all floors

    void set_monitor(monitor *m);  // set the monitor

    void reg_pas(passenger *p);  // if a passenger want to board the elevator, register it (by floor)

    void ding();  // the elevator has arrived at a floor

    void run();  // move elevator up or down

    bool is_full() const;

    bool is_accessible(int floor_id);

    // getters

    class floor *get_current_floor();

    int get_direction() const;

    bool get_status() const;

    int get_ding_stage() const;

    int get_alighting_num(class floor *f);

    int get_free_space() const;

    int get_load() const;

    std::vector<class floor *> get_accessible_floors() const;

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
    long long refresh_time_stamp;  // refresh time stamp for elevator
    long long timer;  // to calculate the running/idle time

    int direction = stop;
    bool status = idle;
    bool full = false;
    int ding_stage = 0;  // indicates at which stage the ding function is running

    class floor *current_floor;
    std::vector<passenger *> passengers;  // passengers on the elevator
    std::vector<class floor *> floors;  // all floors
    std::vector<class floor *> accessible_floors;  // floors accessible by the elevator

    std::map<class floor *, std::vector<passenger *>> registry;  // table of passengers to be boarded/alighted at each floor

    void board();  // board passengers

    void alight();  // alight passengers

    int choose_direction();  // choose the direction of the elevator

    void set_refresh_time();  // set base time for elevator

    long long get_time_gap() const;  // calculate time gap between current time and last run time

    long long get_time();  // get the running/idle time
};


#endif //ELEVATOR_H
