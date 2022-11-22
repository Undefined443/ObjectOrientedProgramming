#include "monitor.h"
#include <memory>
#include <QVector>
#include <QString>


monitor::monitor(building *_building, MainWindow *_main_window) :
    b(_building),
    main_window(_main_window),
    base_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
    refresh_time_stamp(base_time_stamp),
    elevNum(b->conf["elevator.count"]),
    floorNum(b->conf["building.floors"]),
    elevator_status(elevNum, std::vector<int>(4, 0)),  // [elevator]<flag, current floor, direction, load> flag: 0: needn't move 1: need to move
    floor_info(elevNum, std::vector(floorNum, std::vector<int>(3, 0))),  // [elevator][floor]<upside number, downside number, alight number>
    s(elevNum, _main_window)
    {
    b->set_monitor(this);
    // color the accessible floors
    auto groups = b->conf["elevator.group"].get<std::vector<std::vector<int>>>();
    auto accessible_floors = b->conf["elevator.accessibleFloors"].get<std::vector<std::vector<int>>>();
    for (int g = 0; g < groups.size(); ++g) {
        for (auto e: groups[g]) {
            for (auto f: accessible_floors[g]) {
                main_window->set_floor_color(e - 1, f - 1, "#EDFFED");
            }
        }
    }
}

void monitor::run() {
    // static variables, to reduce refresh rate, enhance performance
    static std::vector<int> last_load_info(elevNum, 0);
    static std::vector<std::vector<std::vector<int>>> last_floor_info(elevNum, std::vector(floorNum, std::vector<int>(3, 0)));
    static QVector<QString> last_message(3);
    static std::string last_time;

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
            if (floor_info[i][j] != last_floor_info[i][j]) {
                last_floor_info[i][j] = floor_info[i][j];
                main_window->set_floor_info(i, j, floor_info[i][j][0], floor_info[i][j][1], floor_info[i][j][2]);
            }
        }

        // Set load info
        if (elevator_status[i][3] != last_load_info[i]) {
            last_load_info[i] = elevator_status[i][3];
            if (elevator_status[i][3] == b->conf["elevator.capacity"]) {
                main_window->set_load_info(i, elevator_status[i][3], QString::fromStdString("red"));
            } else {
                main_window->set_load_info(i, elevator_status[i][3], QString::fromStdString("white"));
            }
        }
    }

    // Set time label and refresh statistics
    auto time_passed = set_refresh_time_stamp();
    for (int i = 0; i < b->elevators.size(); ++i) {
        auto e = b->elevators[i];
        if (e->get_status() == elevator::idle) {
            s.add_elevator_idle_time(i, time_passed);
        } else {
            s.add_elevator_running_time(i, time_passed);
        }
    }
    auto seconds_passed = (refresh_time_stamp - base_time_stamp) / 1000;
    auto minutes_passed = seconds_passed / 60;
    seconds_passed %= 60;
    std::string time = "Running: ";
    if (minutes_passed) {
        time += std::to_string(minutes_passed) + " min " + std::to_string(seconds_passed) + " s";
    } else {
        time += std::to_string(seconds_passed) + " s";
    }

    // Set message
    auto pending_message = get_pending_message();
    if (pending_message != last_message) {
        last_message = pending_message;
        main_window->set_message(pending_message);
    }
    if (time != last_time) {
        last_time = time;
        main_window->set_timer(QString::fromStdString(time));
    }
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
            floor_info[i][j][0] = int(floor->upside_boarding_queues[elevator->get_group_id()].size());  // upside number
            floor_info[i][j][1] = int(floor->downside_boarding_queues[elevator->get_group_id()].size());  // downside number
            floor_info[i][j][2] = int(elevator->get_alighting_num(floor));  // alight number
        }
    }
}

QVector<QString> monitor::get_pending_message() {
    int message_duration = b->conf["simulator.messageDurationMillisecond"];
    QVector<QString> ret;
    for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
        auto message_time_stamp = iter->first;
        if (refresh_time_stamp - message_time_stamp > message_duration) {
            iter = messages.erase(iter) - 1;
        } else if (ret.size() < 3) {
            ret.push_back(QString::fromStdString(iter->second));
        }
    }
    while (ret.size() < 3) {
        ret.push_back("");
    }
    return ret;
}

long long monitor::set_refresh_time_stamp() {
    auto current_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    auto ret = current_time_stamp - refresh_time_stamp;
    refresh_time_stamp = current_time_stamp;
    return ret;
}

void monitor::set_status(bool _status) {
    status = _status;
}

bool monitor::get_status() const {
    return status;
}

void monitor::add_passenger_waiting_time(long long time) {
    s.add_passenger_waiting_time(time);
}
