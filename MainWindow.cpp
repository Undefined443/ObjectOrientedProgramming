#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <Chart.h>

MainWindow::MainWindow(int elevator_num, int floor_num, int speed, QWidget *parent) : chart(new Chart(elevator_num)), QWidget(parent) {
    chart->show();

    // Set widgets
    auto elevator_shafts_widget = new QWidget(this);
    auto information_widget = new QWidget(this);
    // Set widget in information widget
    auto message_widget = new QWidget(information_widget);

    // Set layouts
    auto main_window_vertical_layout = new QVBoxLayout(this);  // for this
    auto elevator_shaft_horizontal_layout = new QHBoxLayout(elevator_shafts_widget);  // for elevator shaft widget
    auto information_horizontal_layout = new QHBoxLayout(information_widget);  // for information widget
    auto message_vertical_layout = new QVBoxLayout(message_widget);  // for message widget
    elevator_shaft_horizontal_layout->setSpacing(0);

    // Add elevator shafts
    for (int i = 0; i < elevator_num; ++i) {
        auto elevator_shaft = new ElevatorShaft(floor_num, speed, elevator_shafts_widget);
        //elevator_shaft->setStyleSheet("background-color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;");
        elevator_shaft->show();
        elevator_shaft_horizontal_layout->addWidget(elevator_shaft);
        elevator_shafts.push_back(elevator_shaft);
    }
    elevator_shafts_widget->setLayout(elevator_shaft_horizontal_layout);
    elevator_shafts_widget->setStyleSheet("background-color:white;border:none;border-radius:10px;");
    elevator_shafts_widget->show();

    // Add labels in information widget
    time_label = new QLabel(information_widget);
    time_label->setAlignment(Qt::AlignRight);
    time_label->setStyleSheet("font:\"Times New Roman\";color:black");
    // Set message labels and add them to message widget
    for (int i = 0; i < 3; ++i) {
        auto msg_label = new QLabel(message_widget);
        msg_label->setAlignment(Qt::AlignLeft);
        message_vertical_layout->addWidget(msg_label);
        message_labels.push_back(msg_label);
    }
    message_widget->setLayout(message_vertical_layout);
    message_widget->show();

    // Add message widget and time label
    information_horizontal_layout->addWidget(message_widget);
    information_horizontal_layout->addWidget(time_label);
    information_widget->setLayout(information_horizontal_layout);
    information_widget->show();

    // Add elevator shaft widget and information widget
    main_window_vertical_layout->addWidget(elevator_shafts_widget);
    main_window_vertical_layout->addWidget(information_widget);
    setLayout(main_window_vertical_layout);

    setStyleSheet("background-color:#ECECEC;");

    // Connect signals and slots
    connect(this, &MainWindow::move_elevator_signal, this, &MainWindow::move_elevator_slot);
    connect(this, &MainWindow::floor_info_signal, this, &MainWindow::floor_info_slot);
    connect(this, &MainWindow::message_signal, this, &MainWindow::message_slot);
    connect(this, &MainWindow::timer_signal, this, &MainWindow::timer_slot);
    connect(this, &MainWindow::load_info_signal, this, &MainWindow::load_info_slot);
    connect(this, &MainWindow::floor_color_signal, this, &MainWindow::floor_color_slot);
    connect(this, &MainWindow::elevator_statistics_signal, chart, &Chart::elevator_statistics_slot);
    connect(this, &MainWindow::passenger_statistics_signal, chart, &Chart::passenger_statistics_slot);
}

// Emitters
void MainWindow::move_elevator(int elevator, int start, int end) {
    emit move_elevator_signal(elevator, start, end);
}

void MainWindow::set_floor_info(int elevator, int floor_num, int upside_num, int downside_num, int alight_num) {
    emit floor_info_signal(elevator, floor_num, upside_num, downside_num, alight_num);
}

void MainWindow::set_message(QVector<QString> messages) {
    emit message_signal(messages);
}

void MainWindow::set_timer(QString time) {
    emit timer_signal(time);
}

void MainWindow::set_load_info(int elevator, int load, QString color) {
    emit load_info_signal(elevator, load, color);
}

void MainWindow::set_floor_color(int elevator, int floor_num, QString color) {
    emit floor_color_signal(elevator, floor_num, color);
}

void MainWindow::set_elevator_statistics(int elevator, QVector<long long> elevator_statistics) {
    emit elevator_statistics_signal(elevator, elevator_statistics);
}

void MainWindow::set_passenger_statistics(QVector<long long int> passenger_statistics) {
    emit passenger_statistics_signal(passenger_statistics);
}

// Slots
void MainWindow::move_elevator_slot(int elevator, int start, int end) {
    elevator_shafts[elevator]->move_elevator(start, end);
}

void MainWindow::floor_info_slot(int elevator, int floor_num, int upside_num, int downside_num, int alight_num) {
    elevator_shafts[elevator]->set_floor_info(floor_num, upside_num, downside_num, alight_num);
}

void MainWindow::message_slot(QVector<QString> messages) {
    for (int i = 0; i < messages.size(); ++i) {
        message_labels[i]->setText(messages[i]);
    }
}

void MainWindow::timer_slot(QString time) {
    time_label->setText(time);
}

void MainWindow::load_info_slot(int elevator, int load, QString color) {
    elevator_shafts[elevator]->set_load_info(load, color);
}

void MainWindow::floor_color_slot(int elevator, int floor_num, QString color) {
    elevator_shafts[elevator]->set_floor_color(floor_num, color);
}

void MainWindow::elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics) {
    chart->set_elevator_statistics(elevator, elevator_statistics);
}

void MainWindow::passenger_statistics_slot(QVector<long long> passenger_statistics) {
    chart->set_passenger_statistics(passenger_statistics);
}