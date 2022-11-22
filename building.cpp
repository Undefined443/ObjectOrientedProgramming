#include "building.h"
#include <fstream>
#include <algorithm>

std::random_device building::rd;
std::mt19937 building::e = std::mt19937(rd());

// Once the building instance is created, the constructor will read the configuration file and initialize the building.
building::building() :
    refresh_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
    {
    std::ifstream conf_file("/Users/xiao/CLionProjects/ObjectOrientedProgramming/config.json");
    conf = nlohmann::json::parse(conf_file);
    conf_file.close();
    rand_passenger = std::poisson_distribution<>(conf["simulator.passengerSpawnRate"]);  // spawn passengers per time unit
    // Create floors
    int floor_num = conf["building.floors"];
    for (int i = 1; i <= floor_num; ++i) {
        floors.push_back(new class floor(this, i, conf));
    }
    // Create elevators
    int elevator_num = conf["elevator.count"];
    for (int i = 1; i <= elevator_num; ++i) {
        elevators.push_back(new elevator(i, conf));
    }
    // Configure elevators and floors
    for (auto &elevator : elevators) {
        // Set all floors
        elevator->set_floors(floors);
    }
    // Set elevator groups and attach elevators to floors
    auto groups = conf["elevator.group"].get<std::vector<std::vector<int>>>();
    int initial_floor = conf["elevator.initialFloor"].get<int>() - 1;
    for (int g = 0; g < groups.size(); ++g) {
        // Create elevator group
        std::vector<elevator *> group_elevators;
        for (int el : groups[g]) {
            group_elevators.push_back(elevators[el - 1]);
        }
        for (auto el : groups[g]) {
            auto elevator = elevators[el - 1];
            elevator->set_group_id(g + 1);  // set group id
            elevator->set_group(group_elevators);
            elevator->set_current_floor(floors[initial_floor]);  // set initial floor
            // set elevator's accessible floors
            auto floor_array = conf["elevator.accessibleFloors"][g].get<std::vector<int>>();
            for (auto floor_id : floor_array) {
                auto floor = floors[floor_id - 1];
                elevator->add_accessible_floor(floor);
                floor->add_accessible_elevator(elevator);
            }
        }
    }
}


// Core function
// Building will refresh every second. The refresh function will spawn passengers, update elevators and floors, and update the statistics.
void building::run() {
    // Spawn passengers
    int traffic = conf["simulator.traffic"];
    int time_unit = conf["simulator.timeUnitMillisecond"];
    if (get_time_gap() > time_unit && tot_traffic < traffic) {  // passed 1 time unit since last refresh
        set_refresh_time();
        // generate random number of passengers
        int rand_num = rand_passenger(e);
        tot_traffic += rand_num;
        if (tot_traffic > traffic) {  // reached maximum traffic
            rand_num -= tot_traffic - traffic;
        }
        auto initial_floor = conf["passenger.initialFloor"].get<int>() - 1;
        for (int i = 0; i < rand_num; ++i) {
            auto new_passenger = new passenger(this, floors[initial_floor], conf);
            new_passenger->set_monitor(mon);
            passengers.push_back(new_passenger);
            mon->send_message("<font color=\"green\">Passenger " + std::to_string(new_passenger->get_id()) + " spawned.</font>");
        }
    }
    // Refresh passengers
    for (auto passenger: passengers) {
        passenger->run();
    }
    // Refresh elevators
    for (auto elevator: elevators) {
        elevator->run();
    }
}

void building::remove_passenger(passenger *p) {
    passengers.erase(std::remove(passengers.begin(), passengers.end(), p), passengers.end());
    mon->send_message("<font color=\"blue\">Passenger " + std::to_string(p->get_id()) + " left the building.</font>");
    delete p;
    if (passengers.empty()) {
        mon->send_message("<font color=\"black\">All passengers left the building.</font>");
        mon->set_status(false);
    }
}

void building::set_monitor(monitor *_monitor) {
    mon = _monitor;
    mon->set_status(true);
    for (auto el: elevators) {
        el->set_monitor(_monitor);
    }
    for (auto fl: floors) {
        fl->set_monitor(_monitor);
    }
}

void building::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

long long building::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

nlohmann::json building::get_conf() const {
    return conf;
}
