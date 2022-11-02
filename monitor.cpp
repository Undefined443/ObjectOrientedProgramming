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
        std::string row_padding = "\n\n\n\n";
        std::string gap = "  ";
        std::string head_unit = "+-----+-----+";
        std::string head = head_unit;
        for (int i = 0; i < elevNum - 1; ++i) {
            head += gap +head_unit;
        }
        std::string padding = "          ";
        frame << row_padding << padding << head << '\n';
        for (int i = floorNum - 1; i >= 0; --i) {
            std::ostringstream line;
            line << padding;
            for (int j = 0; j < elevNum; ++j) {
                auto elevator = budin->elevators[j];
                auto direction = elevator->get_direction();
                auto cur_flr = elevator->get_current_floor()->get_id();
                auto prt_flr = i + 1;
                auto printing_floor = budin->floors[i];
                // print elevator shaft
                if (prt_flr == cur_flr + 1) {  // print direction
                    if (direction == elevator::direction::up) {
                        line << "|  \e[32m↑  \e[30m|\e[0m";  // up, green
                    } else if (direction == elevator::direction::down) {
                        line << "|  \e[33m↓  \e[30m|\e[0m";  // down, yellow
                    } else {
                        line << "|     \e[30m|\e[0m";
                    }
                } else if (prt_flr == cur_flr) {  // print elevator symbol
                    if (elevator->is_full()) {
                        line << "|  \e[31m☐  \e[30m|\e[0m";  // full, red
                    } else {
                        switch (elevator->get_ding_stage()) {
                            case 1:  // alighting
                                line << "|  \e[36m☐  \e[30m|\e[0m";  // alighting, cyan
                                break;
                            case 2:  // boarding
                                if (elevator->get_direction() == elevator::direction::up) {
                                    line << "|  \e[32m☐  \e[30m|\e[0m";  // up, green
                                } else {
                                    line << "|  \e[33m☐  \e[30m|\e[0m";  // down, yellow
                                }
                                break;
                            default:
                                line << "|  ☐  \e[30m|\e[0m";
                                break;
                        }
                    }
                } else if (prt_flr == cur_flr - 1 && elevator->get_ding_stage() == 1) {  // print number of alighting passengers
                    int alighting_num = elevator->get_alighting_num(printing_floor);
                    auto num_str = std::to_string(alighting_num);
                    line << "| \e[36m" << std::setw(2) << num_str << "  \e[30m|\e[0m";  // alight, cyan
                } else {
                    bool flag = true;
                    for (auto p: elevator->registry[printing_floor]) {
                        if (p->get_destination() == prt_flr) {
                            line << "|  \e[36m-  \e[30m|\e[0m";  // alighting, cyan
                            flag = false;
                            break;
                        }
                    }
                    if (flag) {
                        line << "|     \e[30m|\e[0m";
                    }
                }
                // print boarding queue
                auto up_que_count = std::to_string(printing_floor->upside_boarding_queues[elevator].size());
                auto down_que_count = std::to_string(printing_floor->downside_boarding_queues[elevator].size());
                if (up_que_count == "0") {
                    up_que_count = "  ";
                }
                if (down_que_count == "0") {
                    down_que_count = "  ";
                }
                if (up_que_count.size() > 2) {
                    up_que_count = "..";
                }
                if (down_que_count.size() > 2) {
                    down_que_count = "..";
                }
                std::stringstream queue;
                queue << "\e[32m" << std::setw(2) << up_que_count << "\e[0m" << " "  // up, green
                << "\e[33m" << std::setw(2) << down_que_count << "\e[0m";  // down, yellow
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
            timer << minutes_passed << "min " << seconds_passed << "s";
        } else {
            timer << seconds_passed << "s";
        }
        frame << std::string(padding.size() + head.size() - timer.str().size(), ' ') << timer.str() << '\n';
        print_msg(frame, padding);
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
            boarding_que[i][j].first = budin->floors[i]->upside_boarding_queues.size();
            boarding_que[i][j].first = budin->floors[i]->downside_boarding_queues.size();
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

void monitor::print_msg(std::ostringstream &frame, std::string &padding) {
    int message_duration = budin->conf["simulator.messageDurationMillisecond"];
    for (auto iter = messages.begin(); iter != messages.end(); ++iter) {
        auto message_time_stamp = iter->first;
        if (refresh_time_stamp - message_time_stamp > message_duration) {
            iter = messages.erase(iter) - 1;
        } else {
            frame << padding << iter->second << '\n';
        }
    }
}

void monitor::force_refresh() {
    refresh_time_stamp = 0;
}
