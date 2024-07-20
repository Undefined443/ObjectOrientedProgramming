#include "elevator.h"
#include "floor.h"
#include <vector>
#include <algorithm>

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
bool elevator::reg_pas(passenger *p) {
    class floor *pas_cur_flr = p->get_current_floor();
    class floor *pas_dst_flr = floors[p->get_destination() - 1];
    registry[pas_cur_flr].push_back(p);  // register passenger's boarding floor
    if (!status) {  // if elevator is idle, start it
        status = true;
        if (current_floor == pas_cur_flr) {  // elevator is at the same floor as the passenger
            direction = pas_dst_flr->get_id() > current_floor->get_id() ? 1 : -1;  // set direction
            ding_stage = 2;
            board();  // board the passenger
            return true;  // no need for other elevators to board the passenger
        } else {  // elevator is not at the same floor as the passenger
            direction = pas_cur_flr->get_id() > current_floor->get_id() ? 1 : -1;  // set direction
            set_refresh_time();  // set a new refresh time
        }
    }
    return false;  // other elevators can board the passenger
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
    auto boarding_queue = current_floor->get_boarding_queue(this);
    int capacity = conf["elevator.capacity"];

    // remove passengers that boarding other elevators in the group
    while (!boarding_queue.empty() && boarding_queue.front()->get_current_elevator() != nullptr && boarding_queue.front()->get_current_elevator() != this) {
        boarding_queue.erase(boarding_queue.begin());
    }

    // board passengers
    while (!boarding_queue.empty() && passengers.size() < capacity) {
        ding_stage = 2;  // keep in the current stage
        passenger *p = boarding_queue.front();
        if (p->get_current_elevator() == nullptr) {  // elevator reached the passenger and remove it from the registry
            for (auto e: group) {
                e->registry[current_floor].erase(
                        std::remove(e->registry[current_floor].begin(), e->registry[current_floor].end(), p),
                        e->registry[current_floor].end());
            }
        }
        if (p->timer(this)) {  // passenger ready to board
            // insert new registry
            registry[floors[p->get_destination() - 1]].push_back(p);
            // passenger enter the elevator
            p->board(this);
            passengers.push_back(p);
            // remove passenger from the boarding queue
            boarding_queue.erase(boarding_queue.begin());  // attention: this is a copy of the original vector
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
    if (boarding_queue.empty() || passengers.size() >= capacity) {  // no more passengers to board
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
            if ((*iter)->timer(this)) {  // passenger ready to alight
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

bool elevator::is_full() const {
    return full;
}

bool elevator::is_accessible(int floor_id) {
    return std::any_of(accessible_floors.cbegin(), accessible_floors.cend(), [floor_id](auto &floor) {
        return floor->get_id() == floor_id;
    });
}

// Setters
void elevator::set_current_floor(class floor *f) {
    current_floor = f;
}

void elevator::set_floors(std::vector<class floor *> &floors) {
    this->floors = floors;
}

// set floors reachable by the elevator, invoke before starting the elevator
void elevator::add_accessible_floor(class floor *f) {
    accessible_floors.push_back(f);
}

void elevator::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

void elevator::set_monitor(monitor *m) {
    mon = m;
}

void elevator::set_group_id(int g_id) {
    group_id = g_id;
}

void elevator::set_group(std::vector<elevator *> &g) {
    group = g;
}

// Getters
class floor *elevator::get_current_floor() {
    return current_floor;
}

int elevator::get_id() const {
    return id;
}

int elevator::get_direction() const {
    return direction;
}

long long elevator::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

bool elevator::get_status() const {
    return status;
}

int elevator::get_ding_stage() const {
    return ding_stage;
}

int elevator::get_alighting_num(class floor *f) {
    return std::accumulate(registry[f].begin(), registry[f].end(), 0, [f](int sum, passenger *p) {
        return sum + (p->get_destination() == f->get_id());
    });
}

int elevator::get_free_space() const {
    return conf["elevator.capacity"].get<int>() - int(passengers.size());
}

int elevator::get_load() const {
    return int(passengers.size());
}

std::vector<class floor *> elevator::get_accessible_floors() const {
    return accessible_floors;
}

int elevator::get_group_id() const {
    return group_id;
}

std::vector<elevator *> elevator::get_group() const {
    return group;
}
