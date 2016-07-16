TEMPLATE = app test
TARGET = station_test

QT += testlib

include (../../common.pro)

HEADERS += station_test.h   \
           ../station.h

SOURCES += station_test.cpp \
           ../station.cpp
