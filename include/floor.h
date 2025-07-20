#ifndef FLOOR_H
#define FLOOR_H

#include <map>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>

#include "building.h"
#include "elevator.h"
#include "monitor.h"
#include "passenger.h"

class elevator;
class passenger;
class monitor;

class floor {
   public:
    friend class monitor;

    floor(class building *_b, int id, const nlohmann::json &conf);

    void add_accessible_elevator(elevator *e);  // set elevators reachable from this floor

    void set_monitor(monitor *_monitor);

    void add_passenger(passenger *p);  // add passenger to the floor, call after passenger alight the elevator &&
                                       // passenger spawned on this floor

    void request_elevator(passenger *p);

    void leave_building(passenger *p);

    std::vector<passenger *> &get_boarding_queue(elevator *e);

    int get_id() const;

    void remove_passenger(passenger *p,
                          elevator *e);  // remove passenger from the floor, call after passenger board the elevator

   private:
    int id;
    nlohmann::json conf;
    building *b;
    monitor *mon;

    std::vector<passenger *> passengers;
    std::vector<elevator *> accessible_elevators;

    // boarding lines, queue the passengers, each elevator has a boarding line
    std::map<int, std::vector<passenger *>> upside_boarding_queues;
    std::map<int, std::vector<passenger *>> downside_boarding_queues;
};

#endif  // FLOOR_H
