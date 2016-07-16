#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QTextCodec>
#include <QTextStream>

#include <station/station.h>

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  Q_UNUSED(app);

  return EXIT_SUCCESS; /*app.exec()*/
}
