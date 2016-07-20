TEMPLATE = app
TARGET = metromap

QT = core  \
     gui

include(common.pro)

HEADERS = \
          exception/exception.h     \
          station/station.h         \
          map/map.h                 \
          ui/mapview.h              \
          ui/metromapmainwindow.h   \
          ui/routeswidget.h         \
          ui/stationwidget.h

SOURCES = \
          station/station.cpp       \
          map/map.cpp               \
          ui/mapview.cpp            \
          ui/metromapmainwindow.cpp \
          ui/routeswidget.cpp       \
          ui/stationwidget.cpp      \
          app/main.cpp

FORMS += \
          ui/mapview.ui             \
          ui/metromapmainwindow.ui  \
          ui/routeswidget.ui        \
          ui/stationwidget.ui

OTHER_FILES = testfiles/*.json
