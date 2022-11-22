/*
 * Monitor the status of the elevators
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "statistics.h"
#include "building.h"
#include "MainWindow.h"
#include <vector>
#include <string>
#include <QVector>
#include <QString>

class building;
class elevator;
class MainWindow;

class monitor {
public:
    monitor(building *_building, MainWindow *_main_window);

    void run();

    void send_message(const std::string &msg);

    void set_status(bool status);

    bool get_status() const;

    void add_elevator_statistic(elevator *e, long long time);

    void add_passenger_waiting_time(long long time);

private:
    building *b;
    int elevNum;
    int floorNum;
    bool status = false;
    statistics s;
    MainWindow *main_window;

    long long base_time_stamp;  // base time stamp for monitor
    long long refresh_time_stamp;  // refresh time stamp for monitor

    std::vector<std::vector<int>> elevator_status;  // [elevator]<flag, current floor, direction, load> flag: 0: needn't move 1: need to move
    std::vector<std::vector<std::vector<int>>> floor_info;  // [elevator][floor]<upside_number, downside_number, alight number>
    std::vector<std::pair<long long, std::string>> messages;

    void get_elevator_status();

    void get_floor_info();

    void set_refresh_time_stamp();

    QVector<QString> get_pending_message();
};

#endif //MONITOR_H
