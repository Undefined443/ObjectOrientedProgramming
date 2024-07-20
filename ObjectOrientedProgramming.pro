QT += widgets charts
TEMPLATE = app
TARGET = ObjectOrientedProgramming
INCLUDEPATH += include

DESTDIR = $$PWD/build
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
UI_DIR = $$PWD/build/ui
RCC_DIR = $$PWD/build/rcc

DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060000

HEADERS += include/building.h \
           include/Chart.h \
           include/elevator.h \
           include/ElevatorShaft.h \
           include/floor.h \
           include/LineChart.h \
           include/MainWindow.h \
           include/monitor.h \
           include/passenger.h \
           include/statistics.h \
           include/nlohmann/json.hpp

SOURCES += src/building.cpp \
           src/Chart.cpp \
           src/elevator.cpp \
           src/ElevatorShaft.cpp \
           src/floor.cpp \
           src/LineChart.cpp \
           src/main.cpp \
           src/MainWindow.cpp \
           src/monitor.cpp \
           src/passenger.cpp \
           src/statistics.cpp

# CONFIG_PATH = $$OUT_PWD
# macx {
#     CONFIG_PATH = $$OUT_PWD/$${TARGET}.app/Contents/MacOS
# }
# win32 {
#     CONFIG_PATH = $$OUT_PWD
# }
# else {
#     error("Unsupported operating system")
# }
# 
# QMAKE_POST_LINK += mkdir -p $$shell_quote($$CONFIG_PATH) $$escape_expand(\\n\\t)
# QMAKE_POST_LINK += cp $$shell_quote($${PWD}/resources/data/config.json) $$shell_quote($$CONFIG_PATH/config.json) $$escape_expand(\\n\\t)
# QMAKE_POST_LINK += cp $$shell_quote($${PWD}/resources/data/data.json) $$shell_quote($$CONFIG_PATH/data.json) $$escape_expand(\\n\\t)
# QMAKE_POST_LINK += mkdir -p $$shell_quote($$CONFIG_PATH/images) $$escape_expand(\\n\\t)
# QMAKE_POST_LINK += cp $$shell_quote($${PWD}/resources/images/elevator.svg) $$shell_quote($$CONFIG_PATH/images/elevator.svg)
