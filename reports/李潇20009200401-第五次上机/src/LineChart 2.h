#ifndef LINECHART_H
#define LINECHART_H

#include <map>
#include <QWidget>
#include <QtCharts>

class monitor;

class LineChart : public QWidget {
public:
    explicit LineChart(QWidget *parent = nullptr);

    void set_data(int elevator_id, std::map<long long, long long> estimated_waiting_time);

private:
    Q_OBJECT
    QLineSeries *lineSeries = nullptr;
    QChart *chart = nullptr;
    QChartView *chartView = nullptr;
//    QValueAxis *axisX = nullptr, *axisY = nullptr;

    signals:
    void set_data_signal(int elevator_id, std::map<long long, long long> estimated_waiting_time);

private slots:
    void set_data_slot(int elevator_id, std::map<long long, long long> estimated_waiting_time);
};


#endif //LINECHART_H
