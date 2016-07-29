TEMPLATE = app
TARGET = metromap

QT = core  \
     gui

include(common.pro)

HEADERS = \
  exception/exception.h      \
  settings/settings.h        \
  station/station.h          \
  map/map.h                  \
  ui/crossoveritem.h         \
  ui/editmodeitems.h         \
  ui/findroutes.h            \
  ui/managebuttons.h         \
  ui/mapview.h               \
  ui/metromapmainwindow.h    \
  ui/railtrackitem.h         \
  ui/selectstation.h         \
  ui/selectstationdialog.h   \
  ui/stationinfo.h           \
  ui/stationitem.h           \
  ui/stationwithcost.h

SOURCES = \
  settings/settings.cpp      \
  station/station.cpp        \
  map/map.cpp                \
  ui/crossoveritem.cpp       \
  ui/editmodeitems.cpp       \
  ui/findroutes.cpp          \
  ui/managebuttons.cpp       \
  ui/mapview.cpp             \
  ui/metromapmainwindow.cpp  \
  ui/railtrackitem.cpp       \
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

CONF_FILES = etc/*.conf
conf.files = $$CONF_FILES
conf.path = $$BUILD_DIR/etc
INSTALLS += conf

OTHER_FILES = testfiles/*.json \
              $$CONF_FILES
