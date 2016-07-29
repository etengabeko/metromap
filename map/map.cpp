#include "map.h"
#include <exception/exception.h>
#include <station/station.h>

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <cmath>

namespace {

double constInfinity() { return 1.0/0.0; }

struct Solution {
  double cost;
  metro::Map::StationIterator from;

  Solution(double acost = constInfinity()) : cost(acost) {}
};

}

namespace metro {

QList<quint32> Map::stationsId() const
{
  return m_stations.keys();
}

QList<quint32> Map::linesId() const
{
  QList<quint32> lines;
  for (StationIterator it = m_stations.constBegin(), end = m_stations.constEnd(); it != end; ++it) {
    quint32 each = it.value()->line();
    if (!lines.contains(each)) {
      lines.append(each);
    }
  }
  return lines;
}

bool Map::containsStation(quint32 id) const
{
  return m_stations.contains(id);
}

const Station& Map::stationById(quint32 id) const
{
  if (!containsStation(id)) {
    throw Exception(QObject::tr("Station #%1 not exists").arg(id));
  }
  return *(m_stations.find(id).value());
}

void Map::insertStation(const Station& station)
{
  QSharedPointer<Station> newStation(new Station(station));
  m_stations.insert(newStation->id(), newStation);
  updateLinks(station);
  rebuildStationsGraph();
}

void Map::updateLinks(const Station& station)
{
  foreach (quint32 neighbour, station.railTracks()) {
    if (m_stations.contains(neighbour)) {
      if (!m_stations[neighbour]->railTracks().contains(station.id())) {
        m_stations[neighbour]->addRailTrack(station.id(), station.railTrackCostTo(neighbour));
      }
    }
  }
  foreach (quint32 neighbour, station.crossOvers()) {
    if (m_stations.contains(neighbour)) {
      if (!m_stations[neighbour]->crossOvers().contains(station.id())) {
        m_stations[neighbour]->addCrossOver(station.id(), station.crossOverCostTo(neighbour));
      }
    }
  }
}

void Map::removeStation(quint32 id)
{
  if (m_stations.contains(id)) {
    removeStation(stationById(id));
  }
}

void Map::removeStation(const Station& station)
{
  if (m_stations.contains(station.id())) {
    removeLinks(station);
    m_stations.remove(station.id());
    rebuildStationsGraph();
  }
}

void Map::removeLinks(const Station& station)
{
  foreach (quint32 neighbour, station.railTracks()) {
    if (m_stations.contains(neighbour)) {
      if (m_stations[neighbour]->railTracks().contains(station.id())) {
        m_stations[neighbour]->removeRailTrack(station.id());
      }
    }
  }
  foreach (quint32 neighbour, station.crossOvers()) {
    if (m_stations.contains(neighbour)) {
      if (m_stations[neighbour]->crossOvers().contains(station.id())) {
        m_stations[neighbour]->removeCrossOver(station.id());
      }
    }
  }
}

void Map::loadFromFile(const QString& fileName)
{
  m_stations.clear();

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

  rebuildStationsGraph();
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

void Map::rebuildStationsGraph()
{
  m_graph.clear();

  for (StationIterator it = m_stations.constBegin(), end = m_stations.constEnd(); it != end; ++it) {
    if (it.value().isNull()) {
      continue;
    }

    QList<StationIterator> ref;
    const Station& each = *(it.value());

    foreach (quint32 id, each.railTracks()) {
      StationIterator found = m_stations.find(id);
      if (found != end) {
        ref.append(found);
      }
    }
    foreach (quint32 id, each.crossOvers()) {
      StationIterator found = m_stations.find(id);
      if (found != end) {
        ref.append(found);
      }
    }

    m_graph.insert(it, ref);
  }
}

QList<Map::StationIterator> Map::findDijkstraPath(const StationIterator& from, const StationIterator& to, qint32 crossoverPenalty) const
{
  Q_ASSERT(containsStation(from.key()));
  Q_ASSERT(containsStation(to.key()));

  QHash<StationIterator, Solution> dist;
  QHash<StationIterator, bool> used;

  foreach (const StationIterator& node, m_graph.keys()) {
    if (node != from) {
      dist[node] = ::constInfinity();
    }
    else {
      dist[node] = 0.0;
    }
    used[node] = false;
  }

  QHashIterator<StationIterator, QList<StationIterator> > it(m_graph);
  while (it.hasNext()) {
    it.next();
    const StationIterator* nearest = 0;
    foreach (const StationIterator& each, m_graph.keys()) {
      if (!used[each] && (nearest == 0 || dist[each].cost < dist[*nearest].cost)) {
        nearest = &each;
      }
      if (nearest == 0) {
        continue;
      }
      if (isinf(dist[*nearest].cost) != 0) {
        continue;
      }
      used[*nearest] = true;
      foreach (const StationIterator& neighbour, m_graph[*nearest]) {
        bool ok = false;
        double cost = nearest->value()->minimumCostTo(neighbour.value()->id(), crossoverPenalty, &ok);
        if (ok) {
          if (dist[*nearest].cost + cost < dist[neighbour].cost) {
            dist[neighbour].cost = dist[*nearest].cost + cost;
            dist[neighbour].from = *nearest;
          }
        }
      }
    }
  }

  QList<StationIterator> resultPath;
  StationIterator step = to;
  while (step != from) {
    if (dist.contains(step) && used[step]) {
      resultPath.push_front(step);
      step = dist[step].from;
    }
    else {
      throw Exception(QObject::tr("Path not found: from %1 to %2").arg(from.value()->name()).arg(to.value()->name()));
    }
  }
  resultPath.push_front(step);
  return resultPath;
}

QList<quint32> Map::findTimeOptimizedPath(quint32 from, quint32 to) const
{
  const qint32 zeroCrossoverPenalty = 0;
  return findPath(from, to, zeroCrossoverPenalty);
}

QList<quint32> Map::findCrossOverOptimizedPath(quint32 from, quint32 to) const
{
  const qint32 customCrossoverPenalty = 10;
  return findPath(from, to, customCrossoverPenalty);
}

QList<quint32> Map::findPath(quint32 from, quint32 to, qint32 crossoverPenalty) const
{
  if (!containsStation(from)) {
    throw Exception(QObject::tr("Station #%1 not exists").arg(from));
  }
  if (!containsStation(to)) {
    throw Exception(QObject::tr("Station #%1 not exists").arg(to));
  }

  QList<quint32> result;
  foreach (const StationIterator& each, findDijkstraPath(m_stations.constFind(from),
                                                         m_stations.constFind(to),
                                                         crossoverPenalty)) {
    result << each.key();
  }

  return result;
}

QString Map::debugString() const
{
  const qint32 zeroCrossoverPenalty = 0;

  QStringList stations;
  for (QHash<StationIterator, QList<StationIterator> >::const_iterator it = m_graph.constBegin(),
       end = m_graph.constEnd();
       it != end; ++it) {
    const Station& from = *(it.key().value());
    QStringList ref;
    QListIterator<StationIterator> ref_it(it.value());
    while (ref_it.hasNext()) {
      const Station& to = *(ref_it.next().value());
      bool ok = false;
      qint32 cost = from.minimumCostTo(to.id(), zeroCrossoverPenalty, &ok);
      ref << QString("%1:%2 => %3").arg(to.id()).arg(to.name()).arg((ok ? QString::number(cost) : QString("NaN")));
    }
    stations << QString("%1:%2 {%3}").arg(from.id()).arg(from.name()).arg(ref.join(", "));
  }

  return stations.join("\n");
}

} // metro
