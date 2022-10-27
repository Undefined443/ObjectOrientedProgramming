#include "floor.h"

floor::floor(class building *b, int id, const nlohmann::json &conf) : budin(b), id(id), conf(conf) {}

void floor::request_elevator(passenger *p) {
    // find an elevator
    static int candidate = 0;
    elevator *nearest_elevator = elevators[candidate++];
    if (candidate >= elevators.size()) {
        candidate = 0;
    }
    // add the passenger to the boarding queue
    if (p->get_destination() > id) {
        upside_boarding_lines[nearest_elevator].push(p);
    } else {
        downside_boarding_lines[nearest_elevator].push(p);
    }
    // register the passenger
    nearest_elevator->reg_pas(p);
}

void floor::set_elevators(std::vector<elevator *> &ev) {
    for (auto e: ev) {
        elevators.push_back(e);
    }
}

std::queue<passenger *> &floor::get_boarding_queue(elevator *e) {
    return e->get_direction() > 0 ? upside_boarding_lines[e] : downside_boarding_lines[e];
}

int floor::get_id() const {
    return id;
}

void floor::add_passenger(passenger *p) {
    passengers.push_back(p);
}

void floor::remove_passenger(passenger *p) {
    passengers.erase(std::remove(passengers.begin(), passengers.end(), p), passengers.end());
}
