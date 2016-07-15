TEMPLATE = app
TARGET = metromap

QT = core  \
#     gui

include(common.pro)

HEADERS = \
          station/station.h          \

SOURCES = \
          station/station.cpp  \
          app/main.cpp

#FORMS =
