#ifndef ELEVATORSHAFT_H
#define ELEVATORSHAFT_H

#include "LineChart.h"
#include "monitor.h"
#include <vector>
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QString>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class monitor;

class ElevatorShaft : public QWidget {
public:
    ElevatorShaft(int id, int floor_num, int speed, QWidget *parent = nullptr);

    void move_elevator(int start, int end);

    void set_floor_info(int floor_num, int upside_num, int downside_num, int alight_num);

    void set_load_info(int load, QString color);

    void set_floor_color(int floor_num, QString color);

    void set_monitor(monitor *mon);

private:
    Q_OBJECT
    int id = -1;
    monitor *m;
    QLabel *elevator_label;
    QPushButton *load_button;
    QVector<QWidget *> floors;
    QVector<QVector<QLabel *>> floor_labels;
    QPropertyAnimation *QAnimation;
    LineChart *line_chart;

    signals:
    void move_elevator_signal(int start, int end);

    void floor_info_signal(int floor_num, int upside_num, int downside_num, int alight_num);

    void load_info_signal(int load, QString color);

    void floor_color_signal(int floor_num, QString color);

private slots:
    void move_elevator_slot(int start, int end);

    void floor_info_slot(int floor_num, int upside_num, int downside_num, int alight_num);

    void load_info_slot(int load, QString color);

    void floor_color_slot(int floor_num, QString color);

    void load_button_clicked_slot();
};

#endif //ELEVATORSHAFT_H
