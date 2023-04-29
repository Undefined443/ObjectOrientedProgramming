#ifndef BUILDINGWIDGET_H
#define BUILDINGWIDGET_H

#include "ElevatorShaft.h"
#include "Chart.h"
#include <string>
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QString>

QT_BEGIN_NAMESPACE
class Chart;
class QWidget;
class QLabel;
QT_END_NAMESPACE


class MainWindow : public QWidget {
public:
    MainWindow(int elevator_num, int floor_num, int speed, QWidget *parent = nullptr);

    void move_elevator(int elevator, int start, int end);

    void set_floor_info(int elevator, int floor_num, int upside_num, int downside_num, int alight_num);

    void set_message(QVector<QString> messages);

    void set_timer(QString time);

    void set_load_info(int elevator, int load, QString color);

    void set_floor_color(int elevator, int floor_num, QString color);

    void set_elevator_statistics(int elevator, QVector<long long> elevator_statistics);

    void set_passenger_statistics(QVector<long long> passenger_statistics);

private:
    Q_OBJECT
    int id = 0;
    std::vector<ElevatorShaft *> elevator_shafts;
    std::vector<QLabel *> message_labels;
    QLabel *time_label;
    Chart *chart;

    signals:
    void move_elevator_signal(int elevator, int start, int end);

    void floor_info_signal(int elevator, int floor_num, int upside_num, int downside_num, int alight_num);

    void message_signal(QVector<QString> messages);

    void timer_signal(QString time);

    void load_info_signal(int elevator, int load, QString color);

    void floor_color_signal(int elevator, int floor_num, QString color);

    void elevator_statistics_signal(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_signal(QVector<long long> passenger_statistics);

private slots:
    void move_elevator_slot(int elevator, int start, int end);

    void floor_info_slot(int elevator, int floor_num, int upside_num, int downside_num, int alight_num);

    void message_slot(QVector<QString> messages);

    void timer_slot(QString time);

    void load_info_slot(int elevator, int load, QString color);

    void floor_color_slot(int elevator, int floor_num, QString color);

    void elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_slot(QVector<long long> passenger_statistics);
};


#endif // BUILDINGWIDGET_H
