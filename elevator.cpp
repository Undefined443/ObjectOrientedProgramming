#include "elevator.h"
#include "floor.h"
#include <vector>

elevator::elevator(int id, const nlohmann::json &conf) : id(id), conf(conf) {}

void elevator::run() {
    if (!status) {  // if elevator is not running
        return;
    } else if (ding_stage) {  // elevator is still alighting/boarding passengers, continue to alight/board
        ding();
    } else {  // elevator is ready to move to the next floor
        int speed = conf["elevator.speed"];
        int time_unit = conf["simulator.timeUnitMillisecond"];
        // if time gap is greater than <speed> second & not boarding or alighting, then move the elevator
        if (get_time_gap() > speed * time_unit) {
            auto next_floor = current_floor->get_id() - 1 + direction;
            if (next_floor < 0 || next_floor >= floors.size()) {
                throw std::out_of_range("elevator " + std::to_string(id) + ": floor index (" + std::to_string(next_floor) + ") out of range.");
            }
            current_floor = floors[next_floor];  // move elevator up or down
            ding();  // elevator arrived at a floor
        }
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
            ding_stage = 2;
            board();  // board the passenger
        } else {  // elevator is not at the same floor as the passenger
            direction = pas_cur_flr->get_id() > current_floor->get_id() ? 1 : -1;  // set direction
            set_refresh_time();  // set a new refresh time
        }
    }
}

// elevator arrived at a floor
void elevator::ding() {
    if (ding_stage == 0) {  // elevator arrived at a new floor, start ding process
        ++ding_stage;
    }
    if (ding_stage == 1) {  // elevator is still alighting passengers, continue to alight
        alight();
    }
    if (ding_stage == 2) {  // elevator is still boarding passengers, continue to board
        board();
    }
    if (ding_stage == 3) {  // finished alight/alight process
        ding_stage = 0;
        // no passenger going to the same direction as the elevator
        if (passengers.empty()) {
            direction = choose_direction();  // choose a new direction
            if (direction == stop) {  // no passengers request the elevator currently
                status = false;  // idle the elevator
            } else {  // direction changed, board passengers on the new direction
                if (!current_floor->get_boarding_queue(this).empty()) {
                    ding_stage = 2;
                    board();
                }
            }
        }
    }
}

// board passengers, invoke in ding()
void elevator::board() {
    // get a boarding queue and board the passengers
    auto &boarding_queue = current_floor->get_boarding_queue(this);
    int capacity = conf["elevator.capacity"];
    while (!boarding_queue.empty() && passengers.size() < capacity) {
        ding_stage = 2;  // keep in the current stage
        passenger *p = boarding_queue.front();
        if (p->timer()) {  // wait for passenger to board
            // remove old registry
            registry[current_floor].erase(
                    std::remove(registry[current_floor].begin(), registry[current_floor].end(), p),
                    registry[current_floor].end());
            // insert new registry
            registry[floors[p->get_destination() - 1]].push_back(p);
            // passenger enter the elevator
            p->board(this);
            passengers.push_back(p);
            // remove passenger from boarding queue
            boarding_queue.pop();
        } else {  // passenger is still boarding, wait for next time
            break;
        }
    }
    // determine if the elevator is full
    if (passengers.size() == capacity) {
        full = true;
    } else {
        full = false;
    }
    if (boarding_queue.empty()) {  // no more passengers to board
        ding_stage = 3;  // move to the next stage
        set_refresh_time();  // elevator is ready to move
    }
}

// alight passengers, invoke in ding()
void elevator::alight() {
    ding_stage = 2;  // move to the next stage (maybe)
    int cur_flr = current_floor->get_id();
    for (auto iter = registry[current_floor].begin(); iter != registry[current_floor].end(); ++iter) {  // scan the registry and alight passengers
        int dst_flr = (*iter)->get_destination();
        if (dst_flr == cur_flr) {  // find a passenger to alight
            ding_stage = 1;  // keep in the current stage
            if ((*iter)->timer()) {  // wait for passenger to alight
                // passenger out the elevator
                (*iter)->alight(current_floor);
                passengers.erase(
                        std::remove(passengers.begin(), passengers.end(), *iter),
                        passengers.end());
                // remove passenger from registry
                iter = registry[current_floor].erase(iter) - 1;
                ding_stage = 2;  // move to the next stage (maybe)
            } else {  // passenger is still alighting, wait for next time
                break;
            }
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

bool elevator::is_full() const {
    return full;
}

int elevator::get_ding_stage() const {
    return ding_stage;
}

int elevator::get_alighting_num(class floor *f) {
    int ret = 0;
    for (auto p: registry[f]) {
        if (p->get_destination() == f->get_id()) {
            ++ret;
        }
    }
    return ret;
}

int elevator::get_free_space() const {
    return conf["elevator.capacity"].get<int>() - int(passengers.size());
}

int elevator::get_load() const {
    return int(passengers.size());
}
