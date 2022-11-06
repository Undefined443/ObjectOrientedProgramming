#ifndef BUILDINGWIDGET_H
#define BUILDINGWIDGET_H

#include "ElevatorShaft.h"
#include <string>
#include <QWidget>
#include <QLabel>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE


class MainWindow : public QWidget {
public:
    MainWindow(int elevator_num, int floor_num, int speed, QWidget *parent = nullptr);

    void move_elevator(int elevator, int start, int end);

    void set_floor_info(int elevator, int floor_num, int v1, int v2, int v3);

    void set_message(std::vector<std::string> messages);

    void set_timer(std::string);

private:
Q_OBJECT
    int id = 0;
    std::vector<ElevatorShaft *> elevator_shafts;
    std::vector<QLabel *> message_labels;
    QLabel *time_label;

    signals:
    void elevator_signal(int elevator, int start, int end);

    void floor_signal(int elevator, int floor_num, int v1, int v2, int v3);

    void message_signal(std::vector<std::string> messages);

    void timer_signal(std::string time);

private slots:
    void elevator_slot(int elevator, int start, int end);

    void floor_slot(int elevator, int floor_num, int v1, int v2, int v3);

    void message_slot(std::vector<std::string> messages);

    void timer_slot(std::string time);
};


#endif // BUILDINGWIDGET_H
