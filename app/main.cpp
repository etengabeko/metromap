#include <QApplication>
#include <QDebug>
#include <QTextCodec>

#include <exception/exception.h>
#include <ui/metromapmainwindow.h>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

  try {
    metro::MetroMapMainWindow mainwindow;
    mainwindow.showMaximized();

    if (mainwindow.init()) {
      return app.exec();
    }
  }
  catch (metro::Exception& e) {
    qDebug() << e.what();
    return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}
