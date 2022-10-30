#include "monitor.h"
#include <iostream>
#include <sstream>

monitor::monitor(building *b) : budin(b), refresh_time_stamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()), base_time_stamp(refresh_time_stamp),
                                refresh_rate(budin->conf["simulator.refreshRateMillisecond"]),
                                elevNum(budin->conf["elevator.count"]), floorNum(budin->conf["building.floors"]) {
    budin->set_monitor(this);
}

void monitor::run() {
    if (get_time_gap() > refresh_rate) {
        set_refresh_time();
        get_elevator_loc();
        get_boarding_que();
        std::ostringstream frame;
        std::system("clear");
        std::string row_padding = "\n\n\n\n\n\n\n";
        std::string gap = "  ";
        std::string head_unit = "+-----+-----+";
        std::string head = head_unit;
        for (int i = 0; i < elevNum - 1; ++i) {
            head += gap +head_unit;
        }
        std::string padding = "             ";
        frame << row_padding << padding << head << '\n';
        for (int i = floorNum - 1; i >= 0; --i) {
            std::ostringstream line;
            line << padding;
            for (int j = 0; j < elevNum; ++j) {
                auto elevator = budin->elevators[j];
                auto direction = elevator->get_direction();
                auto cur_flr = elevator->get_current_floor()->get_id();
                auto printing_floor = i + 1;
                if (printing_floor == cur_flr + 1) {  // print direction
                    if (direction == elevator::direction::up) {
                        line << "|  ↑  |";
                    } else if (direction == elevator::direction::down) {
                        line << "|  ↓  |";
                    } else {
                        line << "|     |";
                    }
                } else if (printing_floor == cur_flr) {  // print elevator symbol
                    line << "|  ☐  |";
                } else {
                    line << "|     |";
                }
                // print boarding queue
                auto current_floor = budin->floors[i];
                auto up_que_count = std::to_string(current_floor->upside_boarding_lines[elevator].size());
                auto down_que_count = std::to_string(current_floor->downside_boarding_lines[elevator].size());
                if (up_que_count == "0") {
                    up_que_count = "  ";
                }
                if (down_que_count == "0") {
                    down_que_count = "  ";
                }
                std::stringstream queue;
                queue << std::setw(2) << up_que_count;
                queue << " ";
                queue << std::setw(2) << down_que_count;
                line << queue.str() << "|" + gap;
            }
            frame << line.str() << "\n";
        }
        frame << padding << head << "\n";
        auto seconds_passed = (refresh_time_stamp - base_time_stamp) / 1000;
        auto minutes_passed = seconds_passed / 60;
        seconds_passed %= 60;
        std::stringstream timer;
        timer << "running: ";
        if (minutes_passed) {
            timer << minutes_passed << "m " << seconds_passed << "s";
        } else {
            timer << seconds_passed << "s";
        }
        frame << std::string(padding.size() + head.size(), ' ') << timer.str() << '\n';
        print_msg(frame);
        std::cout << frame.str();
    }
}

void monitor::print(const std::string &msg) {
    auto time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    messages.emplace_back(time_stamp, msg);
}

void monitor::get_elevator_loc() {

    for (int i = 0; i < elevNum; ++i) {
        elevator_loc[i].first = budin->elevators[i]->get_current_floor()->get_id();
        elevator_loc[i].second = budin->elevators[i]->get_direction();
    }
}

void monitor::get_boarding_que() {
    for (int i = 0; i < floorNum; ++i) {
        for (int j = 0; j < elevNum; ++j) {
            boarding_que[i][j].first = budin->floors[i]->upside_boarding_lines.size();
            boarding_que[i][j].first = budin->floors[i]->downside_boarding_lines.size();
        }
    }
}

void monitor::set_refresh_time() {
    refresh_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}


long long monitor::get_time_gap() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() - refresh_time_stamp;
}

void monitor::set_status(bool _status) {
    status = _status;
}

bool monitor::get_status() const {
    return status;
}

void monitor::print_msg(std::ostringstream &frame) {
    int message_duration = budin->conf["simulator.messageDurationMillisecond"];
    for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
        auto message_time_stamp = iter->first;
        if (refresh_time_stamp - message_time_stamp > message_duration) {
            iter = messages.erase(iter) - 1;
        } else {
            frame << iter->second << '\n';
        }
    }
}

void monitor::force_refresh() {
    refresh_time_stamp = 0;
}
