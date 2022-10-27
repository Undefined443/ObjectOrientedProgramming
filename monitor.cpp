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
    budin->run();
    if (get_time_gap() > refresh_rate) {
        set_refresh_time();
        get_elevator_loc();
        get_boarding_que();
        std::ostringstream frame;
        std::system("clear");
        std::string row_padding = "\n\n\n\n\n\n\n";
        std::string head_padding = "---------------------------------------------------------------------------------------------------------------\n";
        std::string padding = "                   ";
        frame << row_padding << padding << head_padding;
        for (int i = floorNum; i >= 0; --i) {
            std::ostringstream line;
            line << padding;
            for (int j = 0; j < elevNum; ++j) {
                auto elevator = budin->elevators[j];
                auto cur_flr = elevator->get_current_floor()->get_id();
                if (cur_flr == i) {  // print elevator direction
                    switch (elevator->get_direction()) {
                        case 1:
                            line << "| up  ";
                            break;
                        case 0:
                            line << "|stop ";
                            break;
                        case -1:
                            line << "|down ";
                            break;
                    }
                } else if (cur_flr - 1 == i) {  // print elevator location
                    line << "|  *  ";
                } else {
                    line << "|     ";
                }
                // print boarding queue
                if (i == floorNum) {  // top row has no boarding queue
                    line << "     ";
                    continue;
                }
                auto current_floor = budin->floors[i];
                auto up_que_count = current_floor->upside_boarding_lines[elevator].size();
                auto down_que_count = current_floor->downside_boarding_lines[elevator].size();
                line << std::setw(2) << up_que_count;
                line << ";";
                line << std::setw(2) << down_que_count;
                line << "";
            }
            frame << line.str() << "|\n";
        }
        frame << padding << head_padding;
        auto seconds_passed = (refresh_time_stamp - base_time_stamp) / 1000;
        frame << std::string(padding.size() + head_padding.size(), ' ') << "running: " << seconds_passed << "s\n";
        std::cout << frame.str();
    }
}

void monitor::printWarning(elevator *e, const std::string &msg) {
    msg_map[e] = msg;
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
