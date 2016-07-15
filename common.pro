PRO_PATH = $$_PRO_FILE_PWD_
BUILD_DIR = $$PRO_PATH/build_dir

INCLUDEPATH += $$PRO_PATH

OBJECTS_DIR = .o

contains(TEMPLATE, lib) {
  DESTDIR = $$BUILD_DIR/lib
}
contains(TEMPLATE, app) {
  DESTDIR = $$BUILD_DIR/bin
  LIBS += -L$$BUILD_DIR/lib
  contains(TEMPLATE, sys) {
    DESTDIR = $$BUILD_DIR/sbin
  }
  contains(TEMPLATE, test) {
    DESTDIR = ./
  }
}

contains(CONFIG, qt) {
  contains(QT, core) {
    MOC_DIR = .moc
    RCC_DIR = .rcc
  }
  contains(QT, gui) {
    QT += widgets
    UI_DIR = .ui
    INCLUDEPATH += .ui
  }
}

CONFIG += debug    \
          warn_on

QMAKE_CXXFLAGS += -Werror      \
                  -Wall        \
                  -Wextra      \
#                  -std=c++11
