#include "building.h"
#include <fstream>
#include <algorithm>

std::random_device building::rd;
std::mt19937 building::e = std::mt19937(rd());

// Once the building instance is created, the constructor will read the configuration file and initialize the building.
building::building() : refresh_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {
    std::ifstream conf_file("../config.json");
    conf = nlohmann::json::parse(conf_file);
    conf_file.close();
    rand_passenger = std::poisson_distribution<>(conf["simulator.passengerSpawnRate"]);  // spawn passengers per second
    // Create floors
    int floor_num = conf["building.floors"];
    for (int i = 1; i <= floor_num; ++i) {
        floors.push_back(new class floor(this, i, conf));
    }
    // Create elevators
    int elevator_num = conf["elevator.count"];
    for (int i = 1; i <= elevator_num; ++i) {
        auto *el = new elevator(mon, i, conf);
        elevators.push_back(el);
    }
    // Configure floors and elevators
    for (auto floor : floors) {
        floor->set_elevators(elevators);
    }
    for (auto elevator : elevators) {
        elevator->set_floors(floors);
    }
}


// Core function
// Building will refresh every second. The refresh function will spawn passengers, update elevators and floors, and update the statistics.
void building::run() {
    // Spawn passengers
    int traffic = conf["simulator.traffic"];
    if (get_time_gap() > 1000 && tot_traffic < traffic) {  // passed 1 second since last refresh
        set_base_time();
        // generate random number of passengers
        int rand_num = rand_passenger(e);
        tot_traffic += rand_num;
        for (int i = 0; i < rand_num; ++i) {
            passengers.push_back(new passenger(this, floors[0], conf));
        }
    }
    // Refresh elevators
    for (auto elevator : elevators) {
        elevator->run();
    }
    // Refresh passengers
    for (auto passenger : passengers) {
        passenger->run();
    }
}

void building::remove_passenger(passenger &p) {
    passengers.erase(std::remove(passengers.begin(), passengers.end(), &p), passengers.end());
}

void building::set_monitor(monitor *m) {
    mon = m;
}

void building::set_base_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long long building::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}
