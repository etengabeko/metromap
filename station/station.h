#ifndef METROMAP_STATION_STATION_H
#define METROMAP_STATION_STATION_H

#include <QMap>
#include <QString>

class QTextStream;
template <typename T> class QList;

namespace metro {

class Station
{
public:
  Station();

  void setId(quint32 id);
  void setLine(quint32 line);
  void setName(const QString& name);

  void addCrossOver(quint32 stationId, qint32 cost);
  void addNextRailTrack(quint32 stationId, qint32 cost);
  void addPreviousRailTrack(quint32 stationId, qint32 cost);

  quint32 id() const;
  quint32 line() const;
  const QString& name() const;

  bool isValid() const;

  QList<quint32> crossOvers() const;
  QList<quint32> railTracks() const;

  qint32 minimumCostTo(quint32 id, qint32 crossoverPenalty, bool* ok = 0) const;

  QString toJsonString() const;
  static Station fromJsonString(const QString& str);

private:
  QString makeJsonObject(const QString& key, const QStringList& values) const;

  template <typename K, typename V>
  QString makeJsonPair(const K& key, const V& value) const
  {
    return QString("\"%1\": \"%2\"").arg(key).arg(value);
  }

  static QString jsonKey(const QString& jsonPair);
  static QString jsonValue(const QString& jsonPair);

  void addFromJsonContent(QMap<quint32, qint32>& target, QStringList jsonContent);

private:
  quint32 m_id;
  quint32 m_line;
  QString m_name;

  QMap<quint32, qint32> m_crossovers;
  QMap<quint32, qint32> m_railtracksNext;
  QMap<quint32, qint32> m_railtracksPrevious;

};

} // metro

QTextStream& operator<<(QTextStream& out, const metro::Station& station);
QTextStream& operator>>(QTextStream& in, metro::Station& station);

#endif // METROMAP_STATION_STATION_H
