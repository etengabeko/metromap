#include "map.h"
#include <exception/exception.h>
#include <station/station.h>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace metro {

Map::Map()
{
}

void Map::loadFromFile(const QString& fileName)
{
  QFile f(fileName);
  if (!f.open(QFile::ReadOnly)) {
    throw Exception(f.errorString());
  }

  QTextStream in(&f);
  while (!in.atEnd()) {
    QSharedPointer<Station> st(new Station());
    in >> *st;
    m_stations.insert(st->id(), st);
  }
}

void Map::saveToFile(const QString& fileName) const
{
  QFile f(fileName);
  if (!f.open(QFile::WriteOnly)) {
    throw Exception(f.errorString());
  }

  QTextStream out(&f);
  QHashIterator<quint32, QSharedPointer<Station> > it(m_stations);
  while (it.hasNext()) {
    const QSharedPointer<Station>& st = it.next().value();
    if (!st.isNull()) {
      out << *st;
      if (it.hasNext()) {
        out << ",";
      }
      out << "\n";
    }
  }
}

} // metro
