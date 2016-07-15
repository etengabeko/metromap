#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QTextCodec>
#include <QTextStream>

#include <station/station.h>

namespace {

QString testInputFileName()  { return qApp->applicationDirPath() + "/../../testfiles/input.json"; }
QString testOutputFileName() { return qApp->applicationDirPath() + "/../../testfiles/output.json"; }

//metro::Station sennplStation()
//{
//  metro::Station station;
//  station.m_id = 24;
//  station.m_line = 2;
//  station.m_name = QString::fromUtf8("Сенная площадь");
//  station.m_crossovers.insert(41, 3);
//  station.m_crossovers.insert(53, 4);
//  station.m_railtracksNext.insert(25, 2);
//  station.m_railtracksPrevious.insert(23, 1);

//  return station;
//}

//metro::Station volkStation()
//{
//  metro::Station station;
//  station.m_id = 56;
//  station.m_line = 5;
//  station.m_name = QString::fromUtf8("Волковская");
//  station.m_railtracksPrevious.insert(55, 5);

//  return station;
//}

//metro::Station chernStation()
//{
//  metro::Station station;
//  station.m_id = 12;
//  station.m_line = 1;
//  station.m_name = QString::fromUtf8("Чернышевская");
//  station.m_railtracksNext.insert(13, 3);
//  station.m_railtracksPrevious.insert(11, 3);

//  return station;
//}

}

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  Q_UNUSED(app);

  QList<metro::Station> stations;

  QFile f(::testInputFileName());
  if (f.open(QFile::ReadOnly)) {
    QTextStream in(&f);
    while (!in.atEnd()) {
      metro::Station st;
      in >> st;
      stations.append(st);
    }
  }
  else {
    qDebug() << f.fileName() << f.errorString();
  }
  f.close();

  f.setFileName(::testOutputFileName());
  if (f.open(QFile::WriteOnly)) {
    QTextStream out(&f);
    QListIterator<metro::Station> it(stations);
    while (it.hasNext()) {
      out << it.next();
      if (it.hasNext()) {
        out << ",";
      }
      out << "\n";
    }
  }
  else {
    qDebug() << f.fileName() << f.errorString();
  }
  f.close();

  return EXIT_SUCCESS; /*app.exec()*/
}
