#ifndef FLOOR_H
#define FLOOR_H

#include "elevator.h"
#include "passenger.h"
#include "building.h"
#include "monitor.h"
#include "nlohmann/json.hpp"
#include <vector>
#include <queue>
#include <map>

class elevator;
class passenger;
class monitor;

class floor {
public:
    friend class monitor;

    floor(class building *b, int id, const nlohmann::json &conf);

    void set_elevators(std::vector<elevator *> &ev);  // set elevators reachable from this floor

    void set_monitor(monitor *_monitor);

    void add_passenger(passenger *p);  // add passenger to the floor, call after passenger alight the elevator && passenger spawned on this floor

    void request_elevator(passenger *p);

    void leave_building(passenger *p);

    std::queue<passenger *> &get_boarding_queue(elevator *e);

    int get_id() const;

    void remove_passenger(passenger *p);  // remove passenger from the floor, call after passenger board the elevator

private:
    int id;
    nlohmann::json conf;
    building *budin;
    monitor *mon;

    std::vector<passenger *> passengers;
    std::vector<elevator *> elevators;

    // boarding lines, queue the passengers, each elevator has a boarding line
    std::map<elevator *, std::queue<passenger *>> upside_boarding_lines;
    std::map<elevator *, std::queue<passenger *>> downside_boarding_lines;
};


#endif //FLOOR_H
