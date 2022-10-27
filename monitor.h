/*
 * Monitor the status of the elevators
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "building.h"
#include <map>
#include <string>

class building;
class elevator;

class monitor {
public:
    monitor(building *b);

    void run();

    void printWarning(elevator *e, const std::string &msg);

private:
    std::map<elevator *, std::string> msg_map;
    building *budin;
    int refresh_rate;
    std::map<int, std::pair<int, int>> elevator_loc;  // <elevID, <elevFlr, elevDir>>
    std::map<int, std::map<int, std::pair<int, int>>> boarding_que;  // <flr, <elevID, <up_que, down_que>>>

    int elevNum;
    int floorNum;

    long long refresh_time_stamp;
    long long base_time_stamp;

    void get_elevator_loc();

    void get_boarding_que();

    void set_refresh_time();

    long long get_time_gap() const;
};


#endif //MONITOR_H
