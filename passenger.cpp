#include "passenger.h"

// Initialize the static variable
std::random_device passenger::rd;
std::mt19937 passenger::e(rd());

passenger::passenger(class building *b, class floor *f, const nlohmann::json &conf) : refresh_time_stamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        current_building(b), current_floor(f), conf(conf) {

    rand_floor = std::uniform_int_distribution<>(1, conf["building.floors"]);
    rand_boarding_time = std::uniform_int_distribution<>(conf["passenger.boardingTimeRange"][0], conf["passenger.boardingTimeRange"][1]);
    rand_active_time = std::uniform_int_distribution<>(conf["passenger.activeTimeRange"][0], conf["passenger.activeTimeRange"][1]);
    boarding_time = rand_boarding_time(e);  // set random boarding time
    active_time = rand_active_time(e);
    current_floor->add_passenger(this);
}

void passenger::run() {
    // request an elevator after active time
    if (get_time_gap() > active_time * 1000 && !activated) {
        activated = true;
        set_random_dest();
        current_floor->request_elevator(this);
    }
}

void passenger::set_random_dest() {
    destination_floor = rand_floor(e);
    while (destination_floor == original_floor) {
        destination_floor = rand_floor(e);
    }
}

void passenger::leave_building() {
    current_building->remove_passenger(*this);
}

void passenger::board(elevator *el) {
//    sleep(boarding_time * 1000);  // passenger boarding the elevator
    // passenger out the floor
    current_floor->remove_passenger(this);
    current_floor = nullptr;
    current_elevator = el;
}

void passenger::alight(class floor *f) {
//    sleep(boarding_time * 1000);  // passenger alighting the elevator
    // passenger enter the floor
    current_floor = f;
    current_floor->add_passenger(this);
    current_elevator = nullptr;
    active_time = rand_active_time(e);  // reset active time
    set_refresh_time();  // reset base time
    activated = false;  // reset activated
}

void passenger::set_monitor(monitor *m) {
    mon = m;
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
