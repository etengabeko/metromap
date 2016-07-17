#include "map.h"
#include <exception/exception.h>
#include <station/station.h>

#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace metro {

QList<quint32> Map::stationsId() const
{
  return m_stations.keys();
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

  buildStationsGraph();
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

void Map::buildStationsGraph()
{
  typedef QHash<quint32, QSharedPointer<Station> >::const_iterator iter_t;

  for (iter_t it = m_stations.constBegin(), end = m_stations.constEnd(); it != end; ++it) {
    if (it.value().isNull()) {
      continue;
    }

    QList<iter_t> ref;
    const Station& each = *(it.value());

    foreach (quint32 id, each.railTracks()) {
      iter_t found = m_stations.find(id);
      if (found != end) {
        ref.append(found);
      }
    }
    foreach (quint32 id, each.crossOvers()) {
      iter_t found = m_stations.find(id);
      if (found != end) {
        ref.append(found);
      }
    }

    m_graph.insert(it, ref);
  }
}

QString Map::debugString() const
{
  typedef QHash<quint32, QSharedPointer<Station> >::const_iterator iter_t;

  QStringList stations;
  for (QHash<iter_t, QList<iter_t> >::const_iterator it = m_graph.constBegin(),
       end = m_graph.constEnd();
       it != end; ++it) {
    const Station& from = *(it.key().value());
    QStringList ref;
    QListIterator<iter_t> ref_it(it.value());
    while (ref_it.hasNext()) {
      const Station& to = *(ref_it.next().value());
      bool ok = false;
      qint32 cost = from.minimumCostTo(to.id(), &ok);
      ref << QString("%1:%2 => %3").arg(to.id()).arg(to.name()).arg((ok ? QString::number(cost) : QString("NaN")));
    }
    stations << QString("%1:%2 {%3}").arg(from.id()).arg(from.name()).arg(ref.join(", "));
  }

  return stations.join("\n");
}

} // metro
