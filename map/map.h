#ifndef METROMAP_MAP_MAP_H
#define METROMAP_MAP_MAP_H

#include <QHash>
#include <QList>
#include <QSharedPointer>

class QString;

namespace metro {

class Station;

class Map
{
public:
  typedef QHash<quint32, QSharedPointer<Station> >::const_iterator StationIterator;

  QList<quint32> stationsId() const;
  bool containsStation(quint32 id) const;

  const Station& stationById(quint32 id) const;

  void loadFromFile(const QString& fileName);
  void saveToFile(const QString& fileName) const;

  QList<quint32> findTimeOptimizedPath(quint32 from, quint32 to) const;
  QList<quint32> findCrossOverOptimizedPath(quint32 from, quint32 to) const;

  QString debugString() const;

private:
  void buildStationsGraph();
  QList<StationIterator> findDijkstraPath(const StationIterator& from, const StationIterator& to) const;

private:
  QHash<quint32, QSharedPointer<Station> > m_stations;
  QHash<StationIterator, QList<StationIterator> > m_graph;

};

} // metro

inline uint qHash(const QHash<quint32, QSharedPointer<metro::Station> >::const_iterator& key)
{
  return qHash(key.key());
}

#endif // METROMAP_MAP_MAP_H
