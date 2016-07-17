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
  QList<quint32> stationsId() const;
  bool containsStation(quint32 id) const;

  const Station& stationById(quint32 id) const throw();

  void loadFromFile(const QString& fileName) throw();
  void saveToFile(const QString& fileName) const throw();

  QString debugString() const;

private:
  void buildStationsGraph();

private:
  QHash<quint32, QSharedPointer<Station> > m_stations;

  QHash<QHash<quint32, QSharedPointer<Station> >::const_iterator,
        QList<QHash<quint32, QSharedPointer<Station> >::const_iterator> > m_graph;

};

} // metro

inline uint qHash(const QHash<quint32, QSharedPointer<metro::Station> >::const_iterator& key)
{
  return qHash(key.key());
}

#endif // METROMAP_MAP_MAP_H
