#include "monitor.h"
#include <memory>

monitor::monitor(building *_building, MainWindow *_main_window) :
    b(_building),
    main_window(_main_window),
    base_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()),
    elevNum(b->conf["elevator.count"]),
    floorNum(b->conf["building.floors"]),
    elevator_status(elevNum, std::vector<int>(4, 0)), // [elevator]<flag, current floor, direction, load> flag: 0: needn't move 1: need to move
    floor_info(elevNum, std::vector(floorNum, std::vector<int>(3, 0))) { // [elevator][floor]<upside number, downside number, alight number>
    b->set_monitor(this);
}

void monitor::run() {
    get_elevator_status();
    get_floor_info();
    for (int i = 0; i < elevNum; ++i) {
        // Move elevator
        if (elevator_status[i][0] == 1) {
            elevator_status[i][0] = 0;
            auto start = elevator_status[i][1];
            auto end = start + elevator_status[i][2];
            main_window->move_elevator(i, start, end);
        }

        // Set floor info
        for (int j = 0; j < floorNum; ++j) {
            main_window->set_floor_info(i, j, floor_info[i][j][0], floor_info[i][j][1], floor_info[i][j][2]);
        }
    }

    // Set message and time
    set_refresh_time_stamp();
    auto seconds_passed = (refresh_time_stamp - base_time_stamp) / 1000;
    auto minutes_passed = seconds_passed / 60;
    seconds_passed %= 60;
    std::string timer = "running: ";
    if (minutes_passed) {
        timer += std::to_string(minutes_passed) + "min " + std::to_string(seconds_passed) + "s";
    } else {
        timer += std::to_string(seconds_passed) + "s";
    }
    main_window->set_message(get_pending_message());
    main_window->set_timer(timer);
}

void monitor::send_message(const std::string &msg) {
    auto time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    if (messages.size() >= 3) {
        while (messages.size() >= 3) {
            messages.erase(messages.begin());
        }
    }
    messages.emplace_back(time_stamp, msg);
}

void monitor::get_elevator_status() {
    for (int i = 0; i < elevNum; ++i) {
        auto elevator = b->elevators[i];
        if (elevator->get_status() == elevator::status::running && !elevator->get_ding_stage()) {  // elevator is running and not boarding/alighting, we might need to move it
            if (elevator_status[i][1] != elevator->get_current_floor()->get_id() - 1) {  // current floor changed
                elevator_status[i][1] = elevator->get_current_floor()->get_id() - 1;  // update current floor
                elevator_status[i][0] = 1;  // current floor changed, we need to move the elevator
            }
            if (elevator_status[i][2] != elevator->get_direction()) {  // direction changed
                elevator_status[i][2] = elevator->get_direction();  // update direction
                elevator_status[i][0] = 1;  // direction changed, we need to move the elevator
            }
        }
        elevator_status[i][3] = elevator->get_load();  // update load
    }
}

void monitor::get_floor_info() {
    for (int i = 0; i < elevNum; ++i) {
        auto elevator = b->elevators[i];
        for (int j = 0; j < floorNum; ++j) {
            auto floor = b->floors[j];
            floor_info[i][j][0] = int(floor->upside_boarding_queues[elevator].size());  // upside number
            floor_info[i][j][1] = int(floor->downside_boarding_queues[elevator].size());  // downside number
            floor_info[i][j][2] = int(elevator->get_alighting_num(floor));  // alight number
        }
    }
}

std::vector<std::string> monitor::get_pending_message() {
    int message_duration = b->conf["simulator.messageDurationMillisecond"];
    std::vector<std::string> ret;
    for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
        auto message_time_stamp = iter->first;
        if (refresh_time_stamp - message_time_stamp > message_duration) {
            iter = messages.erase(iter) - 1;
        } else if (ret.size() < 3) {
            ret.push_back(iter->second);
        }
    }
    while (ret.size() < 3) {
        ret.push_back("");
    }
    return ret;
}

void monitor::set_refresh_time_stamp() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

void monitor::set_status(bool _status) {
    status = _status;
}

bool monitor::get_status() const {
    return status;
}