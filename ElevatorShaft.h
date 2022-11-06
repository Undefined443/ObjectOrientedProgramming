#ifndef ELEVATORSHAFT_H
#define ELEVATORSHAFT_H

#include <vector>
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ElevatorShaft : public QWidget {
public:
    ElevatorShaft(int floor_num, int speed, QWidget *parent = nullptr);

    void move_elevator(int start, int end);

    void set_floor_info(int floor_num, int v1, int v2, int v3);

private:
    Q_OBJECT
    int id = 0;
    QPropertyAnimation *QAnimation;
    QLabel *elevator_label;
    std::vector<QLabel **> floor_labels;

    signals:
            void elevator_signal(int start, int end);

    void floor_signal(int floor_num, int v1, int v2, int v3);

private slots:
            void elevator_slot(int start, int end);

    void floor_slot(int floor_num, int v1, int v2, int v3);
};

#endif // ELEVATORSHAFT_H
