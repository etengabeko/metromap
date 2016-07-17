#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QTextCodec>

#include <exception/exception.h>
#include <map/map.h>
#include <station/station.h>

namespace {

QString fileNameFromArguments(const QStringList& argv)
{
  QStringList::const_iterator it = argv.constBegin(),
                              end = argv.constEnd();
  if (it == end) {
    throw metro::Exception(QObject::tr("Argv is empty"));
  }
  if (++it == end) {
    throw metro::Exception(QObject::tr("Missing metromap filename"));
  }
  return *it;
}

}

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

  try {
    metro::Map map;
    map.loadFromFile(::fileNameFromArguments(app.arguments()));
    QList<quint32> allStations = map.stationsId();
    QListIterator<quint32> it(allStations);
    while (it.hasNext()) {
      quint32 id = it.next();
      qDebug() << QString("#%1:").arg(id)
               << map.stationById(id).toJsonString();
    }
    qDebug() << QString("Count: %1").arg(allStations.size());
  }
  catch (metro::Exception& e) {
    qDebug() << e.what();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS; /*app.exec()*/
}
