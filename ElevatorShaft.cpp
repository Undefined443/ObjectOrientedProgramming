#include "ElevatorShaft.h"
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QLabel>
#include <vector>
#include <QPropertyAnimation>

const int LINE_WIDTH = 72;
const int LINE_HEIGHT = 25;
const int PADDING = 0;
const int BOTTOM_MARGIN = 35;
const int PIC_WIDTH = LINE_HEIGHT - PADDING;

const QString FLOOR_LABEL_QSS = ":/images/elevator.png";
const QString LOAD_LABEL_QSS = "border:2px groove gray;border-radius:5px;";

ElevatorShaft::ElevatorShaft(int floor_num, int speed, QWidget *parent) : QWidget(parent), floors(floor_num), floor_labels(floor_num) {
    // Set fixed size
    setFixedSize(PIC_WIDTH + LINE_WIDTH, LINE_HEIGHT * floor_num - PADDING + BOTTOM_MARGIN);

    // Set connection
    connect(this, &ElevatorShaft::move_elevator_signal, this, &ElevatorShaft::move_elevator_slot);
    connect(this, &ElevatorShaft::floor_info_signal, this, &ElevatorShaft::floor_info_slot);
    connect(this, &ElevatorShaft::load_info_signal, this, &ElevatorShaft::load_info_slot);
    connect(this, &ElevatorShaft::floor_color_signal, this, &ElevatorShaft::floor_color_slot);

    // Set elevator widget and floors widget
    auto elevator_floors_widget = new QWidget(this);
    auto elevator_widget = new QWidget(elevator_floors_widget);
    auto floors_widget = new QWidget(elevator_floors_widget);
    elevator_widget->setFixedSize(PIC_WIDTH, LINE_HEIGHT * floor_num - PADDING);
    floors_widget->setFixedSize(LINE_WIDTH, LINE_HEIGHT * floor_num - PADDING);
    elevator_widget->setStyleSheet("background-color:white;border:none;");
    floors_widget->setStyleSheet("background-color:#ECECEC;border:none;border-radius:10px;");

    // Set elevator label
    elevator_label = new QLabel(elevator_widget);
    elevator_label->setFixedSize(PIC_WIDTH, PIC_WIDTH);
    QPixmap pixmap("/Users/xiao/CLionProjects/ObjectOrientedProgramming/images/elevator.svg");
    QPixmap scaled = pixmap.scaled(elevator_label->size(), Qt::KeepAspectRatio);
    elevator_label->setPixmap(scaled);
    elevator_label->setGeometry(0, (floor_num - 1) * LINE_HEIGHT, PIC_WIDTH, PIC_WIDTH);
    elevator_label->setStyleSheet("background-color:white;border:none;");

    // Set animation
    QAnimation = new QPropertyAnimation(elevator_label, "geometry");
    QAnimation->setDuration(speed);

    // Set load label
    load_label = new QLabel(this);
    load_label->setNum(0);
    load_label->setAlignment(Qt::AlignCenter);
    load_label->setFixedSize(PIC_WIDTH, PIC_WIDTH);
    load_label->setStyleSheet(LOAD_LABEL_QSS);

    // Set layouts
    auto elevator_shaft_vertical_layout = new QVBoxLayout(this);
    auto elevator_floors_horizontal_layout = new QHBoxLayout(elevator_floors_widget);  // for elevator_floors widget
    auto floors_vertical_layout = new QVBoxLayout(floors_widget);  // for floors widget
    elevator_shaft_vertical_layout->setContentsMargins(0, 0, 0, 0);
    elevator_floors_horizontal_layout->setContentsMargins(0, 0, 0, 0);
    floors_vertical_layout->setContentsMargins(0, 0, 0, 0);
    elevator_floors_horizontal_layout->setSpacing(0);
    floors_vertical_layout->setSpacing(0);

    // Add floor to floors vertical layout
    for (int i = 0; i < floor_num; ++i) {
        auto floor = new QWidget(floors_widget);
        floors[floor_num - 1 - i] = floor;

        // Set floor labels
        auto upside_num = new QLabel(floor);
        auto downside_num = new QLabel( floor);
        auto alight_num = new QLabel(floor);
        upside_num->setAlignment(Qt::AlignCenter);
        downside_num->setAlignment(Qt::AlignCenter);
        alight_num->setAlignment(Qt::AlignCenter);
        upside_num->setStyleSheet("color:green;border:none;");
        downside_num->setStyleSheet("color:brown;border:none;");
        alight_num->setStyleSheet("color:blue;border:none;");

        // Add floor labels to floor labels vector
        floor_labels[floor_num - 1 - i].push_back(upside_num);  // the first floor label is at the top
        floor_labels[floor_num - 1 - i].push_back(downside_num);
        floor_labels[floor_num - 1 - i].push_back(alight_num);

        // Set floor layout and add floor labels to floor layout
        auto floor_layout = new QHBoxLayout(floor);
        floor_layout->setSpacing(0);
        floor_layout->setContentsMargins(0, 0, 0, 0);
        floor_layout->addWidget(upside_num);
        floor_layout->addWidget(downside_num);
        floor_layout->addWidget(alight_num);
        floor->setLayout(floor_layout);

        floor->setStyleSheet("background-color:#ECECEC;border:none;");
        floor->setFixedSize(LINE_WIDTH, LINE_HEIGHT);
        floor->show();

        // Add floor to floors vertical layout
        floors_vertical_layout->addWidget(floor);
    }
    floors_widget->setLayout(floors_vertical_layout);

    // Add elevator and floors widget to elevator_floors horizontal layout
    elevator_floors_horizontal_layout->addWidget(elevator_widget);
    elevator_floors_horizontal_layout->addWidget(floors_widget);
    elevator_floors_widget->setLayout(elevator_floors_horizontal_layout);

    // Add elevator_floors widget and load label to elevator_shaft vertical layout
    elevator_shaft_vertical_layout->addWidget(elevator_floors_widget);
    elevator_shaft_vertical_layout->addWidget(load_label);
    this->setLayout(elevator_shaft_vertical_layout);
}

void ElevatorShaft::move_elevator(int start, int end) {
    emit move_elevator_signal(start, end);
}

void ElevatorShaft::set_floor_info(int floor_num, int upside_num, int downside_num, int alight_num) {
    emit floor_info_signal(floor_num, upside_num, downside_num, alight_num);
}

void ElevatorShaft::set_load_info(int load, QString color) {
    emit load_info_signal(load, color);
}

void ElevatorShaft::set_floor_color(int floor_num, QString color) {
    emit floor_color_signal(floor_num, color);
}

void ElevatorShaft::move_elevator_slot(int start, int end) {
    // Move elevator
    auto top = int(floor_labels.size()) - 1;
    start = LINE_HEIGHT * (top - start);
    end = LINE_HEIGHT * (top - end);
    QAnimation->stop();  // stop previous animation, otherwise it will be buggy
    QAnimation->setStartValue(QRect(0, start, PIC_WIDTH, PIC_WIDTH));
    QAnimation->setEndValue(QRect(0, end, PIC_WIDTH, PIC_WIDTH));
    QAnimation->start();
}

void ElevatorShaft::floor_info_slot(int floor_num, int upside_num, int downside_num, int alight_num) {
    if (floor_num < 0 || floor_num >= floor_labels.size()) {
        throw std::invalid_argument("floor number out of range");
    }
    // Set floor info
    if (upside_num == 0) {
        floor_labels[floor_num][0]->clear();
    } else if (upside_num > 99) {
        floor_labels[floor_num][0]->setText("99+");
    } else {
        floor_labels[floor_num][0]->setNum(upside_num);
    }
    if (downside_num == 0) {
        floor_labels[floor_num][1]->clear();
    } else if (downside_num > 99) {
        floor_labels[floor_num][1]->setText("99+");
    } else {
        floor_labels[floor_num][1]->setNum(downside_num);
    }
    if (alight_num == 0) {
        floor_labels[floor_num][2]->clear();
    } else if (alight_num > 99) {
        floor_labels[floor_num][2]->setText("99+");
    } else {
        floor_labels[floor_num][2]->setNum(alight_num);
    }
}

void ElevatorShaft::load_info_slot(int load, QString color) {
    load_label->setNum(load);
    if (color == "red") {
        load_label->setStyleSheet("color:white;background-color:red;" + LOAD_LABEL_QSS);
    } else {
        load_label->setStyleSheet("color:black;background-color:" + color + ";" + LOAD_LABEL_QSS);
    }
}

void ElevatorShaft::floor_color_slot(int floor_num, QString color) {
    if (floor_num < 0 || floor_num >= floor_labels.size()) {
        throw std::invalid_argument("floor number out of range");
    }
    floors[floor_num]->setStyleSheet("background-color:" + color + ";border:none;border-radius:none;");
}