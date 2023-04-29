#include "floor.h"
#include <stdexcept>
#include <cmath>

floor::floor(class building *_b, int id, const nlohmann::json &conf) : b(_b), id(id), conf(conf) {}

void floor::request_elevator(passenger *p) {
    // find an elevator
    std::vector<std::pair<elevator *, std::pair<int, int>>> candidates;
    int pas_direction = p->get_destination() - id > 0 ? elevator::direction::up : elevator::direction::down;
    for (auto el: accessible_elevators) {
        if (!el->is_accessible(p->get_destination())) {
            continue;
        }
        auto el_status = el->get_status();
        auto el_direction = el->get_direction();
        auto el_cur_flr = el->get_current_floor()->get_id();

        // some temporary variables
        int el_coming_direction = id - el_cur_flr > 0 ? elevator::direction::up : elevator::direction::down;
        auto &boarding_queue = pas_direction > 0 ? upside_boarding_queues[el->get_group_id()] : downside_boarding_queues[el->get_group_id()];

        bool is_at_same_floor = (el_cur_flr == id) && (el_direction == pas_direction)  && (el->get_ding_stage() != 0);  // same floor && same direction && open for boarding
        bool is_pass_by = el_direction == pas_direction && el_coming_direction == el_direction;  // indicates whether the elevator can pass by passenger's floor without changing direction
        bool non_return = !el_status || is_at_same_floor || is_pass_by;  // elevator is idle || at the same floor || elevator can pass by passenger's floor without changing direction
        bool single_return = el_direction != pas_direction;
        int free_space = el->get_free_space() - int(boarding_queue.size());  // free space after boarding other passengers

        if (non_return) {
            int distance = std::abs(el_cur_flr - id);
            candidates.emplace_back(el, std::make_pair(distance, free_space));
        } else if (single_return){
            int distance;
            if (el_direction == elevator::direction::up) {
                distance = conf["building.floors"].get<int>() - id + conf["building.floors"].get<int>() - el_cur_flr;
            } else {
                distance = id - 1 + el_cur_flr - 1;
            }
            candidates.emplace_back(el, std::make_pair(distance, free_space));
        } else {  // double return
            int distance;
            if (el_direction == elevator::direction::up) {
                distance = id - 1 + conf["building.floors"].get<int>() - el_cur_flr + conf["building.floors"].get<int>() - 1;
            } else {
                distance = conf["building.floors"].get<int>() - id + el_cur_flr - 1 + conf["building.floors"].get<int>() - 1;
            }
            candidates.emplace_back(el, std::make_pair(distance, free_space));
        }
    }
    std::sort(candidates.begin(), candidates.end(), [](const std::pair<elevator *, std::pair<int, int>> &a, const std::pair<elevator *, std::pair<int, int>> &b) {
        if (a.second.first == b.second.first) {
            return a.second.second > b.second.second;  // if distance is the same, choose the one with more free space
        }
        return a.second.first < b.second.first;
    });
    auto nearest_elevator = candidates[0].first;

    // add the passenger to the boarding queue
    if (p->get_destination() > id) {
        upside_boarding_queues[nearest_elevator->get_group_id()].push_back(p);
    } else {
        downside_boarding_queues[nearest_elevator->get_group_id()].push_back(p);
    }
    // register the passenger
    for (auto e:nearest_elevator->get_group()) {
        if (e->reg_pas(p)) {
            break;
        }
    }
}

void floor::add_accessible_elevator(elevator *e) {
    accessible_elevators.push_back(e);
}

std::vector<passenger *> &floor::get_boarding_queue(elevator *e) {
    return e->get_direction() > 0 ? upside_boarding_queues[e->get_group_id()] : downside_boarding_queues[e->get_group_id()];
}

int floor::get_id() const {
    return id;
}

void floor::add_passenger(passenger *p) {
    passengers.push_back(p);
}

void floor::remove_passenger(passenger *p, elevator *e) {
    passengers.erase(std::remove(passengers.begin(), passengers.end(), p), passengers.end());
    auto &boarding_queue = get_boarding_queue(e);
    boarding_queue.erase(std::remove(boarding_queue.begin(), boarding_queue.end(), p), boarding_queue.end());
}

void floor::leave_building(passenger *p) {
    if (id == 1) {
        passengers.erase(std::remove(passengers.begin(), passengers.end(), p), passengers.end());
        b->remove_passenger(p);
    } else {
        throw std::runtime_error("Invalid operation: passenger can only leave the building from 1st floor.");
    }
}

void floor::set_monitor(monitor *_monitor) {
    mon = _monitor;
}
