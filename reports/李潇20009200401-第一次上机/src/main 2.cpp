#include "monitor.h"
#include "building.h"

void setup();
void loop();

building *budin;
monitor *mon;

void setup() {
    budin = new building();
    mon = new monitor(budin);
}

void loop() {
    budin->run();
    mon->run();
}

int main() {
    setup();
    while (true) {
        loop();
    }
}
