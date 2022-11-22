#include "monitor.h"
#include "MainWindow.h"
#include "building.h"
#include <thread>
#include <QApplication>

building *b;
monitor *mon;
MainWindow *main_window;

void run() {
    while (mon->get_status()) {
        b->run();
        mon->run();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    b = new building();

    auto conf = b->get_conf();
    int elevator_count = conf["elevator.count"];
    int floor_num = conf["building.floors"];
    int time_unit = conf["simulator.timeUnitMillisecond"];
    int speed = conf["elevator.speed"].get<int>() * time_unit;

    main_window = new MainWindow(elevator_count, floor_num, speed);

    main_window->show();

    mon = new monitor(b, main_window);

    std::thread t(run);

    return QApplication::exec();
}
