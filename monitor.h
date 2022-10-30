/*
 * Monitor the status of the elevators
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "building.h"
#include <queue>
#include <map>
#include <string>

class building;

class elevator;

class monitor {
public:
    monitor(building *_building);

    void run();

    void print(const std::string &msg);

    void set_status(bool status);

    bool get_status() const;

    void force_refresh();

private:
    std::vector<std::pair<long long, std::string>> messages;
    building *budin;
    int refresh_rate;
    std::map<int, std::pair<int, int>> elevator_loc;  // <elevID, <elevFlr, elevDir>>
    std::map<int, std::map<int, std::pair<unsigned long, int>>> boarding_que;  // <flr, <elevID, <up_que, down_que>>>

    int elevNum;
    int floorNum;
    bool status = false;

    long long refresh_time_stamp;
    long long base_time_stamp;

    void get_elevator_loc();

    void get_boarding_que();

    void set_refresh_time();

    long long get_time_gap() const;

    void print_msg(std::ostringstream &frame);
};


#endif //MONITOR_H
