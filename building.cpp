/*
 * Description: This file contains the implementation of the building class.
 */

#include "building.h"
#include <fstream>
#include <iostream>
#include <algorithm>

std::random_device building::rd;  // obtain a random number from hardware
std::mt19937 building::e = std::mt19937(rd());  // random number generator

// Once the building instance is created, the constructor will read the configuration file and initialize the building.
building::building() :
    refresh_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
    base_time_stamp(refresh_time_stamp)
    {
    try {
        std::ifstream conf_file("./config.json");
        conf = nlohmann::json::parse(conf_file);
        conf_file.close();
    } catch (nlohmann::detail::parse_error& e) {
        std::cerr << "Error: File \"config.json\" not found. Put it at the same directory as this executable." << std::endl;
        exit(1);
    }

    rand_passenger_normal = std::poisson_distribution<>(conf["simulator.passengerSpawnRate"][0]);  // spawn passengers per time unit
    rand_passenger_peak = std::poisson_distribution<>(conf["simulator.passengerSpawnRate"][1]);  // spawn passengers per time unit
    rush_hours = conf["simulator.rushHours"].get<std::vector<std::pair<int, int>>>();

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
    auto groups = conf["elevator.groups"].get<std::vector<std::vector<int>>>();
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
        int rand_num;
        // check if it is rush hour
        if (is_rush_hour()) {
            rand_num = rand_passenger_peak(e);
            qDebug() << "rush hour";
        } else {
            rand_num = rand_passenger_normal(e);
        }
        tot_traffic += rand_num;  // update total traffic
        if (tot_traffic > traffic) {  // reached maximum traffic
            rand_num -= tot_traffic - traffic;  // adjust the number of passengers
        }
        auto initial_floor = conf["passenger.initialFloor"].get<int>() - 1;  // passenger initial floor
        // spawn passengers
        for (int i = 0; i < rand_num; ++i) {
            auto new_passenger = new passenger(this, floors[initial_floor], conf);
            new_passenger->set_monitor(mon);  // set monitor
            passengers.push_back(new_passenger);  // add passenger to the building
            mon->send_message("<font color=\"green\">Passenger " + std::to_string(new_passenger->get_id()) + " spawned.</font>");  // send tip message to monitor
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

// remove passenger from the building
void building::remove_passenger(passenger *p) {
    // remove passenger
    passengers.erase(std::remove(passengers.begin(), passengers.end(), p), passengers.end());
    // send tip message to monitor
    mon->send_message("<font color=\"blue\">Passenger " + std::to_string(p->get_id()) + " left the building.</font>");
    delete p;
    // send message if all passengers left the building
    if (passengers.empty() && tot_traffic >= conf["simulator.traffic"]) {
        mon->send_message("<font color=\"black\">All passengers left the building.</font>");
        mon->set_status(false);
    }
}

void building::set_monitor(monitor *_monitor) {
    mon = _monitor;
    mon->set_status(true);  // start monitor
    // set monitor for all elevators
    for (auto el: elevators) {
        el->set_monitor(_monitor);
    }
    // set monitor for all floors
    for (auto fl: floors) {
        fl->set_monitor(_monitor);
    }
}

// set the refresh time stamp to current time
void building::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

// Get the time gap between now and the last refresh time
long long building::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

nlohmann::json building::get_conf() const {
    return conf;
}

// check if it is rush hour
bool building::is_rush_hour() const {
    int time_unit = conf["simulator.timeUnitMillisecond"];
    auto relative_time = (refresh_time_stamp - base_time_stamp);  // time since the start of the simulation
    // return true if it is rush hour
    return std::any_of(rush_hours.begin(), rush_hours.end(), [time_unit, relative_time](const std::pair<int, int> &p) {
        auto start_time = p.first * time_unit;
        auto end_time = p.second * time_unit;
        return relative_time >= start_time && relative_time <= end_time;
    });
}
