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
    load();  // load statistics from file
}

// add elevator idle time
void statistics::add_elevator_idle_time(int elevator, long long time) {
    if (!time) {  // if time is 0, do nothing
        return;
    }
    elevator_statistics[elevator][0][1] += time;  // add time to buffer
    if (elevator_statistics[elevator][0][0] < 36000) {  // we need to refresh faster at the beginning, to have a fluent animation
        if (elevator_statistics[elevator][0][1] >= 10) {
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];  // add time to total
            elevator_statistics[elevator][0][1] = 0;  // clear buffer
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));  // update chart
        }
    } else {
        if (elevator_statistics[elevator][0][1] >= 100) {  // it's time to update the chart
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];  // add time to total
            elevator_statistics[elevator][0][1] = 0;  // clear buffer
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));  // update chart
        }
    }
}

// add elevator running time
void statistics::add_elevator_running_time(int elevator, long long time) {
    if (!time) {  // if time is 0, do nothing
        return;
    }
    elevator_statistics[elevator][1][1] += time;  // add time to buffer
    if (elevator_statistics[elevator][1][0] < 36000) {  // we need to refresh faster at the beginning, to have a fluent animation
        if (elevator_statistics[elevator][1][1] >= 10) {
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];  // add time to total
            elevator_statistics[elevator][1][1] = 0;  // clear buffer
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));  // update chart
        }
    } else {
        if (elevator_statistics[elevator][1][1] >= 100) {  // it's time to update the chart
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];  // add time to total
            elevator_statistics[elevator][1][1] = 0;  // clear buffer
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));  // update chart
        }
    }
}

// add passenger waiting time
void statistics::add_passenger_waiting_time(int elevator, std::pair<long, long> time) {
    // to panel
    auto start_time = time.first;  // start time
    auto end_time = time.second;  // end time
    auto time_gap = end_time - start_time;  // time gap
    passenger_statistics.push_back(time_gap);  // add time gap to statistics
    main_window->set_passenger_statistics(get_passenger_statistics());  // update chart

    // to history
    auto ratio = 1000 / time_unit;  // acceleration ratio
    auto time_division = (start_time - base_timestamp) * ratio / sampling_interval_millisecond * sampling_interval_millisecond;  // round down
    estimated_waiting_time[elevator][time_division].first += time_gap * ratio;  // total += time_gap * ratio
    ++estimated_waiting_time[elevator][time_division].second;  // ++count
}

// get elevator statistics
QVector<long long> statistics::get_elevator_statistics(int elevator) const {
    QVector<long long> result(2, 0);
    result[0] = elevator_statistics[elevator][0][0] + elevator_statistics[elevator][0][1];  // idle time
    result[1] = elevator_statistics[elevator][1][0] + elevator_statistics[elevator][1][1];  // running time
    return result;
}

// get passenger statistics
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

// get estimated waiting time
std::map<long long, long long> statistics::get_estimated_waiting_time(int elevator) {
    if (elevator < 0 || elevator >= elevator_statistics.size()) {
        throw std::range_error("in statistics.cpp: parameter \"elevator\" out of range: " + std::to_string(elevator));
    }
    std::map<long long, long long> ret;  // return value
//    auto ratio = 1000 / time_unit;  // acceleration ratio
    for (auto i: estimated_waiting_time[elevator]) {
//        auto time_division = i.first / ratio;
//        auto total = i.second.first / ratio;
        auto time_division = i.first;  // time division
        auto total = i.second.first;  // total waiting time
        auto count = i.second.second;  // count
		if (count <= 0) {
			// throw std::runtime_error("Division by zero error");
			ret[time_division] = 0;
		} else {
			ret[time_division] = total / count;  // average waiting time
		}
    }
    return ret;
}

// load statistics from file
bool statistics::load() {
    std::ifstream input("./data/data.json");
    if (!input) {  // if file not exist
        return false;
    }
    nlohmann::json data;
    input >> data;  // read data
    input.close();

    estimated_waiting_time = data.get<std::vector<std::map<long long, std::pair<long long, long long>>>>();  // get estimated waiting time
    return true;
}

// save statistics to file
bool statistics::save() {
    std::ofstream output("./data/data.json");
    if (!output.is_open()) {  // if file not exist
        return false;
    }
    nlohmann::json data(estimated_waiting_time);
    output << data.dump(2);  // write data
    output.close();
    return true;
}

// return true if it is rush hour
bool statistics::is_rush_hour(int elevator, long long time) {
    if (elevator <= 0 || elevator > estimated_waiting_time.size()) {
        throw std::out_of_range("in statistics.cpp, is_rush_hour: parameter time (" + std::to_string(elevator) + ") out of range.");
    }
    auto ratio = 1000 / time_unit;  // acceleration ratio
    auto relative_time = (time - base_timestamp) * ratio / sampling_interval_millisecond * sampling_interval_millisecond;  // calculate relative time
    auto start_time = relative_time - 2 * sampling_interval_millisecond;  // start time, 2 time division before relative time
    auto end_time = relative_time + 2 * sampling_interval_millisecond;  // end time, 2 time division after relative time
    if (start_time < 0) {  // fix start time
        start_time = 0;
    }
    // calculate if it is rush hour
    for (auto i = start_time; i <= end_time; i += sampling_interval_millisecond) {  // for each time division
        auto total = estimated_waiting_time[elevator - 1][i].first;  // total waiting time
        auto count = estimated_waiting_time[elevator - 1][i].second;  // count
        auto waiting_time = count != 0 ? total / count : 0;  // average waiting time
        if (waiting_time > 60 * 1000) {  // if average waiting time > 60s
            return true; // it is rush hour
        }
    }
    return false;
}
