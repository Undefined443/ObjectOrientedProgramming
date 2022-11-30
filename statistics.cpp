#include "statistics.h"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>

const int sampling_interval_millisecond = 5000;  // used for estimated_waiting_time

statistics::statistics(int elevator_num, MainWindow *_main_window) :
        elevator_statistics(elevator_num, std::vector<std::vector<long long>>(2, std::vector<long long>(2, 0))),
        main_window(_main_window),
        estimated_waiting_time(elevator_num)
{
    load();
}

void statistics::add_elevator_idle_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][0][1] += time;
    if (elevator_statistics[elevator][0][0] < 36000) {  // we need to refresh faster at the beginning, to have a fluent animation
        if (elevator_statistics[elevator][0][1] >= 10) {
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];
            elevator_statistics[elevator][0][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    } else {
        if (elevator_statistics[elevator][0][1] >= 100) {  // it's time to update the chart
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];
            elevator_statistics[elevator][0][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    }
}

void statistics::add_elevator_running_time(int elevator, long long time) {
    if (!time) {
        return;
    }
    elevator_statistics[elevator][1][1] += time;
    if (elevator_statistics[elevator][1][0] < 36000) {  // we need to refresh faster at the beginning, to have a fluent animation
        if (elevator_statistics[elevator][1][1] >= 10) {
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];
            elevator_statistics[elevator][1][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    } else {
        if (elevator_statistics[elevator][1][1] >= 100) {  // it's time to update the chart
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];
            elevator_statistics[elevator][1][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    }
}

void statistics::add_passenger_waiting_time(int elevator, std::pair<long, long> time) {
    // to panel
    auto start_time = time.first;
    auto end_time = time.second;
    auto time_gap = end_time - start_time;
    passenger_statistics.push_back(time_gap);
    main_window->set_passenger_statistics(get_passenger_statistics());

    // to history
    auto ratio = 1000 / time_unit;  // acceleration ratio
    auto time_division = (start_time - base_timestamp) * ratio / sampling_interval_millisecond * sampling_interval_millisecond;  // round down
    estimated_waiting_time[elevator][time_division].first += time_gap * ratio;  // total += time_gap * ratio
    ++estimated_waiting_time[elevator][time_division].second;  // ++count
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
        mode[(i + 500) / 1000 * 1000]++;  // round to the nearest 1000
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

void statistics::set_base_timestamp(long long time_stamp) {
    base_timestamp = time_stamp;
}

void statistics::set_time_unit(int _time_unit) {
    time_unit = _time_unit;
}

std::map<long long, long long> statistics::get_estimated_waiting_time(int elevator) {
    if (elevator < 0 || elevator >= elevator_statistics.size()) {
        throw std::range_error("in statistics.cpp: parameter \"elevator\" out of range: " + std::to_string(elevator));
    }
    std::map<long long, long long> ret;
//    auto ratio = 1000 / time_unit;  // acceleration ratio
    for (auto i: estimated_waiting_time[elevator]) {
//        auto time_division = i.first / ratio;
//        auto total = i.second.first / ratio;
        auto time_division = i.first;
        auto total = i.second.first;
        auto count = i.second.second;
        ret[time_division] = total / count;
    }
    return ret;
//    std::map<long long, long long> data;
//    data[0] = 5000;
//    data[5000] = 1000;
//    data[10000] = 2000;
//    data[15000] = 3000;
//    data[20000] = 2000;
//    data[25000] = 2000;
//    data[30000] = 4000;
//    data[35000] = 5000;
//    data[40000] = 6000;
//    data[45000] = 10000;
//    data[50000] = 8000;
//    data[55000] = 7000;
//    data[60000] = 5000;
//    data[65000] = 4000;
//    data[70000] = 3000;
//    data[75000] = 3000;
//    data[80000] = 3000;
//    data[85000] = 3000;
//    data[90000] = 2000;
//    data[95000] = 1000;
//    data[100000] = 0;
//    return data;
}

bool statistics::load() {
    std::ifstream input("data.json");
    if (!input) {
        return false;
    }
    nlohmann::json data;
    input >> data;
    input.close();

    estimated_waiting_time = data.get<std::vector<std::map<long long, std::pair<long long, long long>>>>();
    return true;
}

bool statistics::save() {
    std::ofstream output("data.json");
    if (!output.is_open()) {
        return false;
    }
    nlohmann::json data(estimated_waiting_time);
    output << data.dump(2);
    output.close();
    return true;
}

bool statistics::is_rush_hour(int elevator, long long time) {
    if (elevator <= 0 || elevator > estimated_waiting_time.size()) {
        throw std::out_of_range("in statistics.cpp, is_rush_hour: parameter time (" + std::to_string(elevator) + ") out of range.");
    }
    auto ratio = 1000 / time_unit;
    auto relative_time = (time - base_timestamp) * ratio / sampling_interval_millisecond * sampling_interval_millisecond;
    auto start_time = relative_time - 2 * sampling_interval_millisecond;
    auto end_time = relative_time + 2 * sampling_interval_millisecond;
    for (auto i = start_time; i <= end_time; i += sampling_interval_millisecond) {
        auto total = estimated_waiting_time[elevator - 1][i].first;
        auto count = estimated_waiting_time[elevator - 1][i].second;
        auto waiting_time = total / count;
        if (waiting_time > 60 * 1000) {
            return true;
        }
    }
    return false;
}
