#ifndef STATISTICS_H
#define STATISTICS_H

#include <QVector>
#include <map>
#include <vector>

class MainWindow;

class statistics {
   public:
    statistics(int elevator_num, MainWindow *main_window);

    void add_elevator_idle_time(int elevator, long long time);  // add elevator idle time

    void add_elevator_running_time(int elevator, long long time);  // add elevator running time

    void add_passenger_waiting_time(int elevator, std::pair<long, long> time);  // add passenger waiting time

    [[nodiscard("elevator statistics")]] QVector<long long> get_elevator_statistics(
        int elevator) const;  // get elevator statistics

    [[nodiscard("passenger statistics")]] QVector<long long> get_passenger_statistics()
        const;  // get passenger statistics

    void set_base_timestamp(long long time_stamp);  // set base time stamp for statistics

    void set_time_unit(int time_unit);

    std::map<long long, long long> get_estimated_waiting_time(int elevator);  // get estimated waiting time

    bool load();  // load statistics from file

    bool save();  // save statistics to file

    bool is_rush_hour(int elevator, long long time);  // return true if it is rush hour

   private:
    std::vector<std::vector<std::vector<long long>>> elevator_statistics;  // store elevator statistics
    std::vector<long long> passenger_statistics;                           // store passenger statistics
    MainWindow *main_window;
    long long base_timestamp = 0;
    int time_unit = 0;
    std::vector<std::map<long long, std::pair<long long, long long>>> estimated_waiting_time;  // elevator:time:data
                                                                                               // data.first: total
                                                                                               // data.second: count
};

#endif  // STATISTICS_H
