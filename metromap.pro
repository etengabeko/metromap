TEMPLATE = app
TARGET = metromap

QT = core  \
#     gui

include(common.pro)

HEADERS = \
          exception/exception.h \
          station/station.h     \
          map/map.h

SOURCES = \
          station/station.cpp   \
          map/map.cpp           \
          app/main.cpp

OTHER_FILES = testfiles/*.json

#FORMS =
