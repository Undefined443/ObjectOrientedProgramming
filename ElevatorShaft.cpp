#include "ElevatorShaft.h"
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QLabel>
#include <vector>
#include <QPropertyAnimation>

const int LINE_WIDTH = 81;
const int LINE_HEIGHT = 30;
const int CELL_WIDTH = 15;
const int MARGIN = 5;
const int PADDING = 7;
const int PIC_WIDTH = LINE_HEIGHT - PADDING;
const int SPEED_COMPENSATION = 50;  // the elevator moving animation takes longer than the set value (don't know why),
// so the actual set value should be smaller than the desired value

ElevatorShaft::ElevatorShaft(int floor_num, int speed, QWidget *parent) : QWidget(parent), floor_labels(floor_num) {
    // Set elevator widget and floors widget
    auto elevator_widget = new QWidget(this);
    auto floors_widget = new QWidget(this);
    elevator_widget->setFixedSize(PIC_WIDTH, LINE_HEIGHT * floor_num - PADDING);
    floors_widget->setFixedSize(LINE_WIDTH, LINE_HEIGHT * floor_num - PADDING);


    // Set elevator label
    elevator_label = new QLabel(elevator_widget);
    elevator_label->setFixedSize(PIC_WIDTH, PIC_WIDTH);
    QPixmap pixmap("/Users/xiao/CLionProjects/QtTest/images/elevator.svg");
    QPixmap scaled = pixmap.scaled(elevator_label->size(), Qt::KeepAspectRatio);
    elevator_label->setPixmap(scaled);

    // Set layouts
    auto horizontal_layout = new QHBoxLayout(this);  // for this
    auto vertical_layout = new QVBoxLayout(floors_widget);  // for floors widget
    horizontal_layout->setContentsMargins(0, 0, 0, 0);
    vertical_layout->setContentsMargins(0, 0, 0, 0);

    // configure floors widget
    for (int i = 0; i < floor_num; ++i) {
        auto floor = new QWidget(floors_widget);

        // Set floor labels
        auto upside_num = new QLabel(floor);
        auto downside_num = new QLabel( floor);
        auto alight_num = new QLabel(floor);
        upside_num->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        downside_num->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        alight_num->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        upside_num->setStyleSheet("color: green;");
        downside_num->setStyleSheet("color: brown;");
        alight_num->setStyleSheet("color: blue;");
        upside_num->setFixedSize(CELL_WIDTH, LINE_HEIGHT);
        downside_num->setFixedSize(CELL_WIDTH, LINE_HEIGHT);
        alight_num->setFixedSize(CELL_WIDTH, LINE_HEIGHT);

        auto arr = new QLabel *[3];
        arr[0] = upside_num;
        arr[1] = downside_num;
        arr[2] = alight_num;
        floor_labels[floor_num - 1 - i] = arr;  // the first floor label is at the top

        // Set floor layout
        auto floor_layout = new QHBoxLayout(floor);
        floor_layout->setContentsMargins(0, 0, 0, 0);
        floor_layout->addWidget(upside_num);
        floor_layout->addWidget(downside_num);
        floor_layout->addWidget(alight_num);

        floor->setLayout(floor_layout);
        floor->show();
        vertical_layout->addWidget(floor);
    }
    floors_widget->setLayout(vertical_layout);

    horizontal_layout->addWidget(elevator_widget);
    horizontal_layout->addWidget(floors_widget);

    elevator_label->setGeometry(0, (floor_num - 1) * LINE_HEIGHT, PIC_WIDTH, PIC_WIDTH);
    QAnimation = new QPropertyAnimation(elevator_label, "geometry");
    QAnimation->setDuration(speed - SPEED_COMPENSATION);
//    QAnimation->setEasingCurve(QEasingCurve::InQuint);  // ease curve style

    connect(this, &ElevatorShaft::elevator_signal, this, &ElevatorShaft::elevator_slot);
    connect(this, &ElevatorShaft::floor_signal, this, &ElevatorShaft::floor_slot);
    setLayout(horizontal_layout);
    setFixedSize(PIC_WIDTH + MARGIN + LINE_WIDTH, LINE_HEIGHT * floor_num - PADDING);
    setStyleSheet("background-color: white;");
}

void ElevatorShaft::move_elevator(int start, int end) {
    emit elevator_signal(start, end);
}

void ElevatorShaft::set_floor_info(int floor_num, int v1, int v2, int v3) {
    emit floor_signal(floor_num, v1, v2, v3);
}

void ElevatorShaft::elevator_slot(int start, int end) {
    // Move elevator
    auto top = int(floor_labels.size()) - 1;
    start = LINE_HEIGHT * (top - start);
    end = LINE_HEIGHT * (top - end);
    QAnimation->setStartValue(QRect(0, start, PIC_WIDTH, PIC_WIDTH));
    QAnimation->setEndValue(QRect(0, end, PIC_WIDTH, PIC_WIDTH));
    QAnimation->start();
}

void ElevatorShaft::floor_slot(int floor_num, int v1, int v2, int v3) {
    if (floor_num < 0 || floor_num >= floor_labels.size()) {
        throw std::invalid_argument("floor number out of range");
    }
    // Set floor info
    if (v1 == 0) {
        floor_labels[floor_num][0]->clear();
    } else {
        floor_labels[floor_num][0]->setNum(v1);
    }
    if (v2 == 0) {
        floor_labels[floor_num][1]->clear();
    } else {
        floor_labels[floor_num][1]->setNum(v2);
    }
    if (v3 == 0) {
        floor_labels[floor_num][2]->clear();
    } else {
        floor_labels[floor_num][2]->setNum(v3);
    }
}
