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
  ui/managebuttons.h        \
  ui/metromapmainwindow.h   \
  ui/routeswidget.h         \
  ui/selectstationdialog.h  \
  ui/selectstationwidget.h  \
  ui/stationwidget.h        \
  ui/stationwithcost.h

SOURCES = \
  station/station.cpp       \
  map/map.cpp               \
  ui/mapview.cpp            \
  ui/managebuttons.cpp      \
  ui/metromapmainwindow.cpp \
  ui/routeswidget.cpp       \
  ui/selectstationdialog.cpp\
  ui/selectstationwidget.cpp\
  ui/stationwidget.cpp      \
  ui/stationwithcost.cpp    \
  app/main.cpp

FORMS += \
  ui/mapview.ui             \
  ui/managebuttons.ui       \
  ui/metromapmainwindow.ui  \
  ui/routeswidget.ui        \
  ui/selectstationdialog.ui \
  ui/selectstationwidget.ui \
  ui/stationwidget.ui       \
  ui/stationwithcost.ui

OTHER_FILES = testfiles/*.json
