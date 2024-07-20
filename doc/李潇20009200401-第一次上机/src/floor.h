#ifndef FLOOR_H
#define FLOOR_H

#include "elevator.h"
#include "passenger.h"
#include "building.h"
#include "nlohmann/json.hpp"
#include <vector>
#include <queue>
#include <map>

class elevator;
class passenger;

class floor {
public:
    friend class monitor;
    floor(class building *b, int id, const nlohmann::json &conf);
    void set_elevators(std::vector<elevator*> &ev);

    void add_passenger(passenger *p);
    void remove_passenger(passenger *p);
    void request_elevator(passenger *p);

    std::queue<passenger*> &get_boarding_queue(elevator *e);
    int get_id() const;
private:
    int id;
    nlohmann::json conf;
    building *budin;
    std::vector<passenger*> passengers;
    std::vector<elevator*> elevators;

    // boarding lines, queue the passengers, each elevator has a boarding line
    std::map<elevator*, std::queue<passenger*>> upside_boarding_lines;
    std::map<elevator*, std::queue<passenger*>> downside_boarding_lines;
};


#endif //FLOOR_H
