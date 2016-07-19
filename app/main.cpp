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

    QStringList path;
    foreach (quint32 id, map.findTimeOptimizedPath(13, 45)) {
      path << QString("%1:%2").arg(id).arg(map.stationById(id).name());
    }
    qDebug() << path.join(" => ");

    path.clear();
    foreach (quint32 id, map.findCrossOverOptimizedPath(13, 45)) {
      path << QString("%1:%2").arg(id).arg(map.stationById(id).name());
    }
    qDebug() << path.join(" => ");
  }
  catch (metro::Exception& e) {
    qDebug() << e.what();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS; /*app.exec()*/
}
