#ifndef ELEVATORSHAFT_H
#define ELEVATORSHAFT_H

#include <vector>
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QString>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ElevatorShaft : public QWidget {
public:
    ElevatorShaft(int floor_num, int speed, QWidget *parent = nullptr);

    void move_elevator(int start, int end);

    void set_floor_info(int floor_num, int upside_num, int downside_num, int alight_num);

    void set_load_info(int load, QString color);

    void set_floor_color(int floor_num, QString color);

private:
    Q_OBJECT
    int id = 0;
    QLabel *elevator_label;
    QLabel *load_label;
    QVector<QWidget *> floors;
    QVector<QVector<QLabel *>> floor_labels;
    QPropertyAnimation *QAnimation;

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
};

#endif // ELEVATORSHAFT_H
