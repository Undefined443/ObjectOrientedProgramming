#include "statistics.h"
#include <map>
#include <algorithm>

statistics::statistics(int elevator_num, MainWindow *_main_window) :
    elevator_statistics(elevator_num, std::vector<std::vector<long long>>(2, std::vector<long long>(2, 0))),
    main_window(_main_window) {}

void statistics::add_elevator_idle_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][0][1] += time;
    if (elevator_statistics[elevator][0][1] >= 100) {  // it's time to update the chart
        elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];
        elevator_statistics[elevator][0][1] = 0;
        main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
    }
}

void statistics::add_elevator_running_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][1][1] += time;
    if (elevator_statistics[elevator][1][1] >= 100) {  // it's time to update the chart
        elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];
        elevator_statistics[elevator][1][1] = 0;
        main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
    }
}

void statistics::add_passenger_waiting_time(long long time) {
    passenger_statistics.push_back(time);
    main_window->set_passenger_statistics(get_passenger_statistics());
}

QVector<long long> statistics::get_elevator_statistics(int elevator) const {
    QVector<long long> result(2, 0);
    result[0] = elevator_statistics[elevator][0][0] + elevator_statistics[elevator][0][1];
    result[1] = elevator_statistics[elevator][1][0] + elevator_statistics[elevator][1][1];
    return result;
}

QVector<long long> statistics::get_passenger_statistics() const {
    QVector<long long> result(4, 0);
    // get average waiting time
    result[0] = std::accumulate(passenger_statistics.begin(), passenger_statistics.end(), 0LL) / (long long)passenger_statistics.size();

    // get max waiting time
    result[1] = *std::max_element(passenger_statistics.begin(), passenger_statistics.end());

    // get mode
    std::map<long long, int> mode;
    for (auto &i: passenger_statistics) {
        mode[i]++;
    }
    result[2] = std::max_element(mode.begin(), mode.end(), [](const auto &a, const auto &b) {
        return a.second < b.second;
    })->first;

    // get median
    std::vector<long long> temp(passenger_statistics);
    std::sort(temp.begin(), temp.end());
    result[3] = temp[temp.size() / 2];

    return result;
}
