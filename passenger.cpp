#include "passenger.h"

// Initialize the static variable
int passenger::c_id = 0;
std::random_device passenger::rd;
std::mt19937 passenger::e(rd());

passenger::passenger(class building *b, class floor *f, const nlohmann::json &conf) : current_building(b), current_floor(f), conf(conf), id(++c_id) {
    rand_floor = std::uniform_int_distribution<>(1, conf["building.floors"]);
    rand_boarding_time = std::uniform_int_distribution<>(conf["passenger.boardingTimeRange"][0], conf["passenger.boardingTimeRange"][1]);
    rand_active_time = std::uniform_int_distribution<>(conf["passenger.activeTimeRange"][0], conf["passenger.activeTimeRange"][1]);
    rand_total_destination = std::uniform_int_distribution<>(conf["passenger.totalDestinationRange"][0], conf["passenger.totalDestinationRange"][1]);
    boarding_time = rand_boarding_time(e);  // set random boarding time
    active_time = rand_active_time(e);
    total_destinations = rand_total_destination(e);
    original_floor = current_floor->get_id();
    current_floor->add_passenger(this);
    set_refresh_time();
}

void passenger::run() {
    // request an elevator after active time
    int time_unit = conf["simulator.timeUnitMillisecond"];
    if (get_time_gap() > active_time * time_unit && !activated) {
        activated = true;
        set_random_dest();
        if (current_floor == nullptr) {
            throw std::runtime_error("passenger " + std::to_string(id) + ": current floor is null.");
        }
        current_floor->request_elevator(this);
    }
}

bool passenger::timer() {
    if (!is_timing) {  // if timer is not running, start the timer
        is_timing = true;
        timer_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return false;
    } else {  // if timer is running, check if time is up
        int time_unit = conf["simulator.timeUnitMillisecond"];
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (current_time - timer_time_stamp > boarding_time * time_unit) {  // if time is up, return true and stop the timer
            is_timing = false;
            return true;
        }
        else {
            return false;
        }
    }
}

void passenger::set_random_dest() {
    if (count_destinations == total_destinations) {  // if the passenger has visited all the floors, leave the building
        destination_floor = 1;
        return;
    }
    destination_floor = rand_floor(e);
    while (destination_floor == original_floor) {
        destination_floor = rand_floor(e);
    }
    if (count_destinations == total_destinations - 1) {  // the last floor the passenger visit can't be the 1st floor
        while (destination_floor == 1) {
            destination_floor = rand_floor(e);
        }
    }
}

void passenger::board(elevator *el) {
    // passenger out the floor
    current_floor->remove_passenger(this);
    current_floor = nullptr;
    current_elevator = el;
}

void passenger::alight(class floor *f) {
    ++count_destinations;
    // passenger enter the floor
    current_floor = f;
    current_elevator = nullptr;
    current_floor->add_passenger(this);
    original_floor = f->get_id();
    destination_floor = 0;
    active_time = rand_active_time(e);  // reset active time
    activated = false;  // reset activated
    set_refresh_time();  // reset refresh time

    if (count_destinations == total_destinations + 1) {
        current_floor->leave_building(this);
    }
}

void passenger::set_monitor(monitor *_monitor) {
    mon = _monitor;
}

void passenger::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

long long passenger::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

class floor *passenger::get_current_floor() {
    return current_floor;
}

int passenger::get_destination() const {
    return destination_floor;
}

int passenger::get_id() const {
    return id;
}
