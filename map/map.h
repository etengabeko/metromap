#ifndef METROMAP_MAP_MAP_H
#define METROMAP_MAP_MAP_H

#include <QHash>
#include <QSharedPointer>

class QString;
template <typename T> class QList;

namespace metro {

class Station;

class Map
{
public:
  Map();

  QList<quint32> stationsId() const;
  bool containsStation(quint32 id) const;

  const Station& stationById(quint32 id) const throw();

  void loadFromFile(const QString& fileName) throw();
  void saveToFile(const QString& fileName) const throw();

private:
  QHash<quint32, QSharedPointer<Station> > m_stations;

};

} // metro

#endif // METROMAP_MAP_MAP_H
