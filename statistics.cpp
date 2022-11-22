#include "statistics.h"
#include <map>
#include <algorithm>

statistics::statistics(int elevator_num, MainWindow *_main_window) :
    elevator_statistics(elevator_num, std::vector<std::vector<long long>>(2, std::vector<long long>())),
    main_window(_main_window) {}

void statistics::add_elevator_idle_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][0].push_back(time);
    auto sum = std::accumulate(elevator_statistics[elevator][0].begin() + 1, elevator_statistics[elevator][0].end(), 0LL);
    if (sum >= 100) {
        elevator_statistics[elevator][0][0] += sum;
        elevator_statistics[elevator][0].erase(elevator_statistics[elevator][0].begin() + 1, elevator_statistics[elevator][0].end());
        main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
    }
}

void statistics::add_elevator_running_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][1].push_back(time);
    auto sum = std::accumulate(elevator_statistics[elevator][1].begin() + 1, elevator_statistics[elevator][1].end(), 0LL);
    if (sum >= 100) {
        elevator_statistics[elevator][1][0] += sum;
        elevator_statistics[elevator][1].erase(elevator_statistics[elevator][1].begin() + 1, elevator_statistics[elevator][1].end());
        main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
    }
}

void statistics::add_passenger_waiting_time(long long time) {
    passenger_statistics.push_back(time);
    main_window->set_passenger_statistics(get_passenger_statistics());
}

QVector<long long> statistics::get_elevator_statistics(int elevator) const {
    QVector<long long> result(2, 0);
    result[0] = std::accumulate(elevator_statistics[elevator][0].begin(), elevator_statistics[elevator][0].end(), 0LL);
    result[1] = std::accumulate(elevator_statistics[elevator][1].begin(), elevator_statistics[elevator][1].end(), 0LL);
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
