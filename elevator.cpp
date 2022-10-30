#include "elevator.h"
#include "floor.h"
#include <vector>
#include <queue>

elevator::elevator(int id, const nlohmann::json &conf) : id(id), conf(conf) {}

void elevator::run() {
    if (!status) {  // if elevator is not running
        return;
    }
    int speed = conf["elevator.speed"];
    int time_unit = conf["simulator.timeUnitMillisecond"];
    if (get_time_gap() > speed * time_unit) {  // if time gap is greater than <speed> second
        auto next_floor = current_floor->get_id() + direction;
        if (next_floor < 1 || next_floor > floors.size()) {
            throw std::runtime_error("Invalid operation: index out of range.");
        }
        current_floor = floors[next_floor - 1];  // move elevator up or down
        ding();  // elevator arrived at a floor
        set_refresh_time();  // reset refresh time
    }
}

// register passenger's boarding floors and alighting floors, invoke by floor
void elevator::reg_pas(passenger *p) {
    class floor *pas_cur_flr = p->get_current_floor();
    class floor *pas_dst_flr = floors[p->get_destination() - 1];
    registry[pas_cur_flr].push_back(p);  // register passenger's boarding floor
    if (!status) {  // if elevator is idle, start it
        status = true;
        if (current_floor == pas_cur_flr) {  // elevator is at the same floor as the passenger
            direction = pas_dst_flr->get_id() > current_floor->get_id() ? 1 : -1;  // set direction
            board();  // board the passenger
        } else {  // elevator is not at the same floor as the passenger
            direction = pas_cur_flr->get_id() > current_floor->get_id() ? 1 : -1;  // set direction
        }
        set_refresh_time();  // set a new refresh time
    }
}

// elevator arrived at a floor
void elevator::ding() {
    // alight passengers
    alight();
    // board passengers
    board();
    // determine a new status and direction
    if (passengers.empty()) {  // no passenger going to the same direction
        direction = choose_direction();  // choose a new direction
        if (direction == stop) {  // no passengers request the elevator currently
            status = false;  // idle the elevator
        } else {  // direction changed, board passengers on the new direction
            board();
        }
    }
}

// board passengers, invoke in ding()
void elevator::board() {
    // get a boarding queue and board the passengers
    auto &boarding_queue = current_floor->get_boarding_queue(this);
    int capacity = conf["elevator.capacity"];
    while (passengers.size() < capacity && !boarding_queue.empty()) {
        passenger *p = boarding_queue.front();
        // remove old registry
        registry[current_floor].erase(std::remove(registry[current_floor].begin(), registry[current_floor].end(), p),
                                      registry[current_floor].end());
        // insert new registry
        registry[floors[p->get_destination() - 1]].push_back(p);
        // passenger enter the elevator
        p->board(this);
        passengers.push_back(p);
        // remove passenger from boarding queue
        boarding_queue.pop();
    }
    // print warning if the elevator is full and there are still passengers in the boarding queue
//    if (passengers.size() == capacity && !current_floor->get_boarding_queue(this).empty()) {
//        mon->print("Elevator E" + std::to_string(id) + " full");
//    }
}

// alight passengers, invoke in ding()
void elevator::alight() {
    int cur_flr = current_floor->get_id();
    for (auto iter = registry[current_floor].begin(); iter != registry[current_floor].end(); ++iter) {  // scan the registry and alight passengers
        int dst_flr = (*iter)->get_destination();
        if (dst_flr == cur_flr) {  // arrive at destination
            // passenger out the elevator
            (*iter)->alight(current_floor);
            passengers.erase(
                    std::remove(passengers.begin(), passengers.end(), *iter),
                    passengers.end());
            // remove passenger from registry
            iter = registry[current_floor].erase(iter) - 1;
        }
    }
}

// choose direction of the elevator, invoke in ding()
int elevator::choose_direction() {
    int ret = 0;
    for (auto &p: registry) {  // scan the registry
        if (!p.second.empty()) {
            int dest_flr = p.first->get_id();
            int crt_flr = current_floor->get_id();
            if ((dest_flr - crt_flr) * direction > 0) {  // destination floor is at the same direction as the elevator is going
                return direction;  // elevator continue to move in the same direction
            } else {
                ret = -1 * direction;  // no same direction registry, change direction
            }
        }
    }
    return ret;
}

// set floors reachable by the elevator, invoke before starting the elevator
void elevator::set_floors(std::vector<class floor *> &fv) {
    for (class floor *f: fv) {
        floors.push_back(f);
    }
    current_floor = floors[conf["elevator.initialFloor"].get<int>() - 1];
}

// getters
class floor *elevator::get_current_floor() {
    return current_floor;
}

int elevator::get_direction() const {
    return direction;
}

void elevator::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

long long elevator::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

bool elevator::get_status() const {
    return status;
}

void elevator::set_monitor(monitor *m) {
    mon = m;
}
