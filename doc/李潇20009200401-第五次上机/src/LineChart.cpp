#include "LineChart.h"
#include <QHBoxLayout>

LineChart::LineChart(QWidget *parent) :
    QWidget(parent),
    lineSeries(new QLineSeries()),
    chart(new QChart()),
    chartView(new QChartView(chart))
//    axisX(new QValueAxis()),
//    axisY(new QValueAxis())
{
    chart->legend()->hide();
    chart->addSeries(lineSeries);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    chart->addAxis(axisY, Qt::AlignLeft);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(400, 300);

    connect(this, &LineChart::set_data_signal, this, &LineChart::set_data_slot);
}

void LineChart::set_data(int elevator_id, std::map<long long, long long> estimated_waiting_time) {
    emit set_data_signal(elevator_id, estimated_waiting_time);
}

void LineChart::set_data_slot(int elevator_id, std::map<long long, long long> estimated_waiting_time) {
    lineSeries->clear();
//    auto maximum_width = estimated_waiting_time.rbegin()->first;
//    auto maximum_height = 0;
    for (auto i: estimated_waiting_time) {
        auto time_division = i.first;
        auto average_waiting_time = i.second;
        lineSeries->append(qreal(time_division / 1000), qreal(average_waiting_time / 1000));
//        if (average_waiting_time > maximum_height) {
//            maximum_height = average_waiting_time;
//        }
    }
    chart->setTitle("E" + QString::number(elevator_id) + " Estimated Waiting Time");
    chart->removeSeries(lineSeries);
    chart->addSeries(lineSeries);
    chart->createDefaultAxes();
//    axisX->setRange(0, maximum_width / 1000);
//    axisY->setRange(0, qreal(maximum_height / 1000) * 1.1);

    chartView->show();
}

