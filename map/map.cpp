#include "map.h"
#include <exception/exception.h>
#include <station/station.h>

#include <QFile>
#include <QList>
#include <QString>
#include <QTextStream>

namespace metro {

Map::Map()
{
}

QList<quint32> Map::stationsId() const
{
  QList<quint32> keys = m_stations.keys();
  qSort(keys);
  return keys;
}

bool Map::containsStation(quint32 id) const
{
  return m_stations.contains(id);
}

const Station& Map::stationById(quint32 id) const throw()
{
  if (!containsStation(id)) {
    throw Exception(QObject::tr("Station #%1 not exists").arg(id));
  }
  return *(m_stations.find(id).value());
}

void Map::loadFromFile(const QString& fileName) throw()
{
  QFile f(fileName);
  if (!f.open(QFile::ReadOnly)) {
    throw Exception(f.errorString());
  }

  QTextStream in(&f);
  while (!in.atEnd()) {
    QSharedPointer<Station> st(new Station());
    in >> *st;
    if (st->isValid()) {
      m_stations.insert(st->id(), st);
    }
  }
}

void Map::saveToFile(const QString& fileName) const throw()
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
