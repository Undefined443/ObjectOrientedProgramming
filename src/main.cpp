#include "monitor.h"
#include "MainWindow.h"
#include "building.h"
#include <thread>
#include <QApplication>

building *b;
monitor *mon;
MainWindow *main_window;

// logic thread
void run() {
    while (mon->get_status()) {  // while the simulation is running
        b->run();  // refresh the building
        mon->run();  // refresh the monitor
    }
    mon->finish();  // finish the simulation
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // create a QApplication

    b = new building();  // create a building

    auto conf = b->get_conf();  // get the configuration of the building
    int elevator_count = conf["elevator.count"];  // get the number of elevators
    int floor_num = conf["building.floors"];  // get the number of floors
    int time_unit = conf["simulator.timeUnitMillisecond"];  // get the time unit
    int speed = conf["elevator.speed"].get<int>() * time_unit;  // get the speed of the elevator

    main_window = new MainWindow(elevator_count, floor_num, speed);  // create a MainWindow

    main_window->show();  // show the MainWindow

    mon = new monitor(b, main_window);  // create a monitor

    std::thread t(run);  // create a thread for logic

    auto ret = QApplication::exec();  // start the QApplication

    // Close button clicked
    t.join();

    delete b;
    delete mon;

    return ret;
}
