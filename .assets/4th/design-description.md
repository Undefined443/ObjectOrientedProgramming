# 第四次上机设计说明

下面说明我根据第四次上机作业的要求完成的设计，以及其他对第三次上机程序的改进。

## 上机内容

电梯运行统计分析

仿真结束后，对电梯在仿真过程中的运行情况进行统计分析。

- 统计各梯的运行与空闲时间；
- 统计各乘客发出乘梯要求后的等待时间；仿真结束后显示这些时间。

### 程序设计

为了统计电梯的运行与空闲时间，以及乘客发出乘梯要求后的等待时间，我给 `elevator` 类和 `passenger` 类分别加了一个变量，用于存储计算运行时间、等待时间所需的时间戳。同时，我设计了一个 `statistic` 类用于统计所需信息，以及一个 `Chart` 类用于显示统计信息。

对于 `elevator` 类，在每次运行 `elevator::run()` 函数时都会计算本次运行与上次运行之间的时间间隔，并将该时间间隔传递给 `statistic` 对象。`statistic` 对象会记录 每个 `elevator` 对象的总空闲时间以及总运行时间，并选择合适的时机要求 `Chart` 对象刷新显示的数据。

> 上机题目里说明的是在仿真结束后显示统计数据，但是我觉得程序本身运行时间较长，如果改为实时显示统计数据的话效果会更好，因此我将程序设计成了实时显示统计数据的模式。

elevator.cpp:

```cpp
void elevator::run() {
    // elevator 在每次刷新自己的状态时都将这次刷新与上次刷新之间的时间间隔传递给 monitor 对象
    mon->add_elevator_statistic(this, get_statistic_time());
    // 其余代码省略
}

// 获取当前时间与上次刷新时间之间的时间间隔
long long elevator::get_statistic_time() {
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();  // 获取当前时间戳
    auto time_gap = current_time - statistic_time_stamp;  // 获取时间间隔
    statistic_time_stamp = current_time;  // 更新 statistic 时间戳
    return time_gap;  // 返回时间间隔
}
```

对于 `passenger` 类，当 `passenger` 对象开始请求电梯时会获取一个时间戳，当 `passenger` 对象开始登梯时会再次获取一个时间戳，并计算两时间戳之间的时间间隔，然后将该时间间隔传递给 `statistic` 对象。`statistic` 对象会统计每个 `passenger` 对象的登梯时间，并计算出平均值、最大值、众数、中位数，然后交由 `Chart` 对象显示数据。

passenger.cpp:

```cpp
void passenger::run() {
    // 当到达激活时间后，passenger 会产生一个乘梯请求
    int time_unit = conf["simulator.timeUnitMillisecond"];
    if (get_time_gap() > active_time * time_unit && !activated) {
        activated = true;
        // 乘客开始等待电梯，获取当前时间戳
        waiting_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        set_random_dest();  // 乘客随机选择一个目的楼层
        if (current_floor == nullptr) {
            throw std::runtime_error("passenger " + std::to_string(id) + ": current floor is null.");
        }
        // 乘客所在楼层相应乘客的乘梯请求
        current_floor->request_elevator(this);
    }
}

// 乘客登梯/下梯时用到的计时器，当乘客开始登梯/下梯时被调用，返回乘客是否完成登梯/下梯动作
bool passenger::timer(elevator *el) {
    if (!is_timing) {  // 如果还没开始计时，则开始计时
        is_timing = true;
        current_elevator = el;
        // 获取开始登梯/下梯时的时间戳
        timer_time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        // 计算乘客等待时间
        auto waiting_time = timer_time_stamp - waiting_timestamp;
        mon->add_passenger_waiting_time(waiting_time);  // 更新统计数据
        return false;
    } else {  // 如果已经开始计时，计时器是否到时
        int time_unit = conf["simulator.timeUnitMillisecond"];
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (current_time - timer_time_stamp > boarding_time * time_unit) {  // 如果计时器到时，则停止计时并返回 true
            is_timing = false;
            return true;
        }
        else {
            return false;
        }
    }
}
```

`statistic` 类用于统计电梯运行数据和乘客等待时间数据。

statistic.h:

```cpp
#ifndef STATISTICS_H
#define STATISTICS_H

#include "MainWindow.h"
#include <vector>
#include <QVector>

class statistics {
public:
    statistics(int elevator_num, MainWindow *main_window);  // 构造函数

    void add_elevator_idle_time(int elevator, long long time);  // 增加一条电梯空闲时间数据

    void add_elevator_running_time(int elevator, long long time);  // 增加一条电梯运行时间数据

    void add_passenger_waiting_time(long long time);  // 增加一条电梯等待时间数据

    [[nodiscard("elevator statistics")]] QVector<long long> get_elevator_statistics(int elevator) const;  // 获取电梯运行时间的统计信息

    [[nodiscard("passenger statistics")]] QVector<long long> get_passenger_statistics() const;  // 获取乘客等待时间的统计信息

private:
    std::vector<std::vector<std::vector<long long>>> elevator_statistics;  // 存储电梯运行时间的统计信息
    std::vector<long long> passenger_statistics;  // 存储乘客等待时间的统计信息
    MainWindow *main_window;  // 指向主窗口的指针
};

#endif //STATISTICS_H
```

statistic.cpp:

```cpp
#include "statistics.h"
#include <map>
#include <algorithm>

statistics::statistics(int elevator_num, MainWindow *_main_window) :
    elevator_statistics(elevator_num, std::vector<std::vector<long long>>(2, std::vector<long long>(2, 0))),
    main_window(_main_window) {}

// 增加一条电梯空闲时间数据
void statistics::add_elevator_idle_time(int elevator, long long time) {
    if (!time) {  // 若时间为 0，则不记录
        return;
    }
    elevator_statistics[elevator][0][1] += time;  // 先将时间加入到缓冲区
    if (elevator_statistics[elevator][0][0] < 36000) {  // 在电梯运行总时间小于 36 秒时加快缓冲区刷新速度，以流畅地显示统计数据更新情况
        if (elevator_statistics[elevator][0][1] >= 10) {  // 当缓冲区的时间大于等于 10 毫秒时，刷新缓冲区，并更新饼图
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];
            elevator_statistics[elevator][0][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    } else {  // 在电梯运行总时间大于等于 36 秒时，缓冲区刷新速度降低，以减少主线程的负担
        if (elevator_statistics[elevator][0][1] >= 100) {  // 当缓冲区的时间大于等于 100 毫秒时，刷新缓冲区，并更新饼图
            elevator_statistics[elevator][0][0] += elevator_statistics[elevator][0][1];
            elevator_statistics[elevator][0][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    }
}

// 增加一条电梯运行时间数据
void statistics::add_elevator_running_time(int elevator, long long time) {
    if (!time) {  // 若时间为 0，则不记录
        return;
    }
    elevator_statistics[elevator][1][1] += time;
    if (elevator_statistics[elevator][1][0] < 36000) {  // 在电梯运行总时间小于 36 秒时加快缓冲区刷新速度，以流畅地显示统计数据更新情况
        if (elevator_statistics[elevator][1][1] >= 10) {  // 当缓冲区的时间大于等于 10 毫秒时，刷新缓冲区，并更新饼图
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];
            elevator_statistics[elevator][1][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    } else {  // 在电梯运行总时间大于等于 36 秒时，缓冲区刷新速度降低，以减少主线程的负担
        if (elevator_statistics[elevator][1][1] >= 100) {  // 当缓冲区的时间大于等于 100 毫秒时，刷新缓冲区，并更新饼图
            elevator_statistics[elevator][1][0] += elevator_statistics[elevator][1][1];
            elevator_statistics[elevator][1][1] = 0;
            main_window->set_elevator_statistics(elevator, get_elevator_statistics(elevator));
        }
    }
}

// 增加一条乘客等待时间数据
void statistics::add_passenger_waiting_time(long long time) {
    passenger_statistics.push_back(time);
    main_window->set_passenger_statistics(get_passenger_statistics());
}

// 获取电梯运行时间统计信息
QVector<long long> statistics::get_elevator_statistics(int elevator) const {
    QVector<long long> result(2, 0);
    result[0] = elevator_statistics[elevator][0][0] + elevator_statistics[elevator][0][1];
    result[1] = elevator_statistics[elevator][1][0] + elevator_statistics[elevator][1][1];
    return result;
}

// 获取乘客等待时间统计信息
QVector<long long> statistics::get_passenger_statistics() const {
    QVector<long long> result(4, 0);
    // 计算均值
    result[0] = std::accumulate(passenger_statistics.begin(), passenger_statistics.end(), 0LL) / (long long)passenger_statistics.size();

    // 计算最大值
    result[1] = *std::max_element(passenger_statistics.begin(), passenger_statistics.end());

    // 计算众数
    std::map<long long, int> mode;
    for (auto &i: passenger_statistics) {
        mode[(i + 500) / 1000 * 1000]++;  // round to the nearest 1000
    }
    result[2] = std::max_element(mode.begin(), mode.end(), [](const auto &a, const auto &b) {
        return a.second < b.second;
    })->first;

    // 计算中位数
    std::vector<long long> temp(passenger_statistics);
    std::sort(temp.begin(), temp.end());
    result[3] = temp[temp.size() / 2];

    return result;
}
```

`Chart` 类用于显示统计数据。

Chart.h:

```cpp
#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class Chart : public QWidget {
public:
    friend class MainWindow;

    Chart(int _elevator_num, QWidget *parent = nullptr);

    // 更新电梯运行时间统计数据
    void set_elevator_statistics(int elevator, QVector<long long> elevator_statistics);

    // 更新乘客等待时间统计数据
    void set_passenger_statistics(QVector<long long> passenger_statistics);

private:
    Q_OBJECT
    int elevator_num;  // 电梯数
    QVector<QPieSeries *> pie_series_vector;  // 显示电梯运行时间统计数据的饼图
    QBarSet *bar_set;  // 显示乘客等待时间统计数据的柱状图
    QValueAxis *axisY;  // 柱状图的纵坐标轴

    signals:
    void elevator_statistics_signal(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_signal(QVector<long long> passenger_statistics);

private slots:
    void elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_slot(QVector<long long> passenger_statistics);
};

#endif // CHART_H
```

Chart.cpp:

```cpp
#include "Chart.h"
#include <exception>
#include <QtCharts>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QtWidgets>
#include <QVector>

const int COLUMN = 5;
const int HEIGHT = 250;

Chart::Chart(int _elevator_num, QWidget *parent) :
    elevator_num(_elevator_num),
    pie_series_vector(elevator_num),
    bar_set(new QBarSet("")),
    axisY(new QValueAxis()),
    QWidget(parent) {

    // 设置 widgets
    auto elevator_panel = new QWidget(this);

    // 设置 layout
    auto grid_layout = new QGridLayout(elevator_panel);

    QVector<QChartView *> chart_view_vector(elevator_num);

    // 设置电梯运行时间统计数据的饼图
    for (int i = 0; i < elevator_num; ++i) {
        chart_view_vector[i] = new QChartView(elevator_panel);
        pie_series_vector[i] = new QPieSeries(chart_view_vector[i]);

        chart_view_vector[i]->setMinimumHeight(HEIGHT);
        chart_view_vector[i]->setMinimumWidth(HEIGHT);

        pie_series_vector[i]->setHoleSize(0.35);

        chart_view_vector[i]->setRenderHint(QPainter::Antialiasing);
        chart_view_vector[i]->chart()->setTitle("E" + QString::number(i + 1));
        chart_view_vector[i]->chart()->addSeries(pie_series_vector[i]);
        chart_view_vector[i]->chart()->legend()->setAlignment(Qt::AlignBottom);
        chart_view_vector[i]->chart()->setTheme(QChart::ChartThemeBlueCerulean);
        chart_view_vector[i]->chart()->legend()->setFont(QFont("Arial", 10));

        grid_layout->addWidget(chart_view_vector[i], i / COLUMN, i % COLUMN);
    }
    elevator_panel->setLayout(grid_layout);

    // 设置乘客等待时间统计数据的柱状图
    auto series = new QBarSeries();
    series->append(bar_set);

    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Passenger Waiting Time");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Mean" << "Maximum" << "Mode" << "Median";

    auto axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    auto chart_view = new QChartView(chart);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setMinimumHeight(HEIGHT);

    grid_layout->addWidget(chart_view, elevator_num / COLUMN, elevator_num % COLUMN, 1, 2);

    setLayout(grid_layout);

    // 初始化
    // 电梯饼图
    for (int i = 0; i < elevator_num; ++i) {
        pie_series_vector[i]->append("Idle: 0 s", 1);
        pie_series_vector[i]->append("Running: 0 s", 1);
    }
    // 乘客柱状图
    *bar_set << 0 << 0 << 0 << 0;
    axisY->setRange(0, 1);
    axisY->setLabelFormat("%.1f s");

    // 连接信号和槽
    connect(this, &Chart::elevator_statistics_signal, this, &Chart::elevator_statistics_slot);
    connect(this, &Chart::passenger_statistics_signal, this, &Chart::passenger_statistics_slot);
}

// Emitters
void Chart::set_elevator_statistics(int elevator, QVector<long long> elevator_statistics) {
    emit elevator_statistics_signal(elevator, elevator_statistics);
}

void Chart::set_passenger_statistics(QVector<long long> passenger_statistics) {
    emit passenger_statistics_signal(passenger_statistics);
}

// Slots

// 更新电梯运行时间统计数据的饼图
void Chart::elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics) {
    if ((long long) pie_series_vector[elevator]->slices()[0]->value() != elevator_statistics[0]) {  // 获取到新的电梯空闲数据
        pie_series_vector[elevator]->slices()[0]->setValue(qreal(elevator_statistics[0]));
        auto idle_time_str = "Idle: " + QString::number(elevator_statistics[0] / 1000) + " s";
        if (pie_series_vector[elevator]->slices()[0]->label() != idle_time_str) {  // 判断是否需要更新 label 内容
            pie_series_vector[elevator]->slices()[0]->setLabel(idle_time_str);
        }
    }
    if ((long long) pie_series_vector[elevator]->slices()[1]->value() != elevator_statistics[1]) {  // 获取到新的电梯运行数据
        pie_series_vector[elevator]->slices()[1]->setValue(qreal(elevator_statistics[1]));
        auto running_time_str = "Running: " + QString::number(elevator_statistics[1] / 1000) + " s";
        if (pie_series_vector[elevator]->slices()[1]->label() != running_time_str) {  // 判断是否需要更新 label 内容
            pie_series_vector[elevator]->slices()[1]->setLabel(running_time_str);
        }
    }
}

// 更新乘客等待时间统计数据的柱状图
void Chart::passenger_statistics_slot(QVector<long long> passenger_statistics) {
    if (passenger_statistics.size() != 4) {
        throw std::invalid_argument("QVector size must be 4");
    }
    auto new_value = qreal(passenger_statistics[1]) / 1000.0;  // 先刷新最大值
    if (bar_set->at(1) != new_value) {
        if (new_value * 1.1 > axisY->max()) {  // 判断是否需要更新 Y 轴范围
            if (axisY->labelFormat() == "%.1f s" && new_value * 1.1 >= 10) {
                axisY->setLabelFormat("%d s");
            }
            axisY->setRange(0, new_value * 1.1);  // 设置新的 Y 轴范围
        }
        bar_set->replace(1, new_value);
    }
    // 刷新其他数据
    for (int i = 0; i < 4; ++i) {
        if (i == 1) {
            continue;
        }
        new_value = qreal(passenger_statistics[i]) / 1000.0;
        if (new_value != bar_set->at(i)) {
            bar_set->replace(i, new_value);
        }
    }
}
```

## 运行结果

![pic](assets/4-1.gif)

![pic](assets/4-2.gif)
