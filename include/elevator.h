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

    bool reg_pas(passenger *p);  // if a passenger want to board the elevator, register it (by floor)

    void ding();  // the elevator has arrived at a floor

    void run();  // move elevator up or down

    [[nodiscard]] bool is_full() const;  // return true if the elevator is full

    bool is_accessible(int floor_id);  // return true if the floor is accessible by the elevator

    void set_group_id(int g_id);  // set group id

    void set_group(std::vector<elevator *> &group);  // set group members

    // getters

    class floor *get_current_floor();  // get current floor

    [[nodiscard]] int get_id() const;  // get elevator id

    [[nodiscard]] int get_direction() const;  // get direction

    [[nodiscard]] bool get_status() const;  // get elevator status

    [[nodiscard]] int get_ding_stage() const;  // get ding stage

    int get_alighting_num(class floor *f);  // get alighting number of the floor

    [[nodiscard]] int get_free_space() const;  // get elevator free space

    [[nodiscard]] int get_load() const;  // get elevator load

    [[nodiscard]] std::vector<class floor *> get_accessible_floors() const;  // get accessible floors

    [[nodiscard]] int get_group_id() const;  // get group id

    [[nodiscard]] std::vector<elevator *> get_group() const;  // get group members

    enum direction {
        up = 1, down = -1, stop = 0
    };

    enum status {
        running = 1, idle = 0
    };

private:
    int id;
    int group_id = 0;
    monitor *mon = nullptr;
    nlohmann::json conf;  // configuration data
    long long refresh_time_stamp = 0;  // refresh time stamp for elevator
    long long statistic_time_stamp;  // to calculate the running/idle time

    int direction = stop;
    bool status = idle;
    bool full = false;
    int ding_stage = 0;  // indicates at which stage the ding function is running

    class floor *current_floor = nullptr;
    std::vector<passenger *> passengers;  // passengers on the elevator
    std::vector<class floor *> floors;  // all floors
    std::vector<class floor *> accessible_floors;  // floors accessible by the elevator
    std::vector<elevator *> group;

    std::map<class floor *, std::vector<passenger *>> registry;  // table of passengers to be boarded/alighted at each floor

    void board();  // board passengers

    void alight();  // alight passengers

    int choose_direction();  // choose the direction of the elevator

    void set_refresh_time();  // set base time for elevator

    [[nodiscard]] long long get_time_gap() const;  // calculate time gap between current time and last run time

    long long get_statistic_time();  // get the running/idle time
};

#endif //ELEVATOR_H
