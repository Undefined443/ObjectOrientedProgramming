#ifndef STATISTICS_H
#define STATISTICS_H

#include "MainWindow.h"
#include <vector>
#include <QVector>

class Chart;

class statistics {
public:
    statistics(int elevator_num, MainWindow *main_window);

    void add_elevator_idle_time(int elevator, long long time);

    void add_elevator_running_time(int elevator, long long time);

    void add_passenger_waiting_time(long long time);

    [[nodiscard("elevator statistics")]] QVector<long long> get_elevator_statistics(int elevator) const;

    [[nodiscard("passenger statistics")]] QVector<long long> get_passenger_statistics() const;

private:
    std::vector<std::vector<std::vector<long long>>> elevator_statistics;
    std::vector<long long> passenger_statistics;
    MainWindow *main_window;
};

#endif //STATISTICS_H
