#ifndef STATISTICS_H
#define STATISTICS_H

#include "MainWindow.h"
#include <vector>
#include <map>
#include <QVector>

class Chart;
class MainWindow;

class statistics {
public:
    statistics(int elevator_num, MainWindow *main_window);

    void add_elevator_idle_time(int elevator, long long time);

    void add_elevator_running_time(int elevator, long long time);

    void add_passenger_waiting_time(int elevator, std::pair<long, long> time);

    [[nodiscard("elevator statistics")]] QVector<long long> get_elevator_statistics(int elevator) const;

    [[nodiscard("passenger statistics")]] QVector<long long> get_passenger_statistics() const;

    void set_base_timestamp(long long time_stamp);

    void set_time_unit(int time_unit);

    std::map<long long, long long> get_estimated_waiting_time(int elevator);

    bool load();

    bool save();

private:
    std::vector<std::vector<std::vector<long long>>> elevator_statistics;
    std::vector<long long> passenger_statistics;
    MainWindow *main_window;
    long long base_timestamp = 0;
    int time_unit = 0;
    std::vector<std::map<long long, std::pair<long long, long long>>> estimated_waiting_time;  // elevator:time:data
                                                                                               // data.first: total
                                                                                               // data.second: count
};

#endif //STATISTICS_H
