#ifndef METROMAP_MAP_MAP_H
#define METROMAP_MAP_MAP_H

#include <QHash>
#include <QSharedPointer>

class QString;

namespace metro {

class Station;

class Map
{
public:
  Map();

  void loadFromFile(const QString& fileName);
  void saveToFile(const QString& fileName) const;

private:
  QHash<quint32, QSharedPointer<Station> > m_stations;

};

} // metro

#endif // METROMAP_MAP_MAP_H
