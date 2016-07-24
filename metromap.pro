TEMPLATE = app
TARGET = metromap

QT = core  \
     gui

include(common.pro)

HEADERS = \
  exception/exception.h      \
  station/station.h          \
  map/map.h                  \
  ui/findroutes.h            \
  ui/managebuttons.h         \
  ui/mapview.h               \
  ui/metromapmainwindow.h    \
  ui/selectstation.h         \
  ui/selectstationdialog.h   \
  ui/stationinfo.h           \
  ui/stationitem.h           \
  ui/stationwithcost.h

SOURCES = \
  station/station.cpp        \
  map/map.cpp                \
  ui/findroutes.cpp          \
  ui/managebuttons.cpp       \
  ui/mapview.cpp             \
  ui/metromapmainwindow.cpp  \
  ui/selectstation.cpp       \
  ui/selectstationdialog.cpp \
  ui/stationinfo.cpp         \
  ui/stationitem.cpp         \
  ui/stationwithcost.cpp     \
  app/main.cpp

FORMS += \
  ui/findroutes.ui           \
  ui/managebuttons.ui        \
  ui/mapview.ui              \
  ui/metromapmainwindow.ui   \
  ui/selectstation.ui        \
  ui/selectstationdialog.ui  \
  ui/stationinfo.ui          \
  ui/stationwithcost.ui

OTHER_FILES = testfiles/*.json
