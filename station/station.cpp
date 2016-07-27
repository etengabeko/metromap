#include "station.h"

#include <QList>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>

namespace {

enum json_keys_t
{
  STATION,
  ID,
  LINE,
  NAME,
  RAILTRACK,
  CROSSOVER
};

const QMap<json_keys_t, QString>& jsonKeys()
{
  static QMap<json_keys_t, QString> jsonKeys;
  if (jsonKeys.isEmpty()) {
    jsonKeys.insert(STATION,   "station"  );
    jsonKeys.insert(ID,        "id"       );
    jsonKeys.insert(LINE,      "line"     );
    jsonKeys.insert(NAME,      "name"     );
    jsonKeys.insert(RAILTRACK, "railtrack");
    jsonKeys.insert(CROSSOVER, "crossover");
  }
  return jsonKeys;
}

QStringList addTabLevel(const QStringList& list)
{
  QRegExp re("[\\t]+");

  QStringList result;
  QStringListIterator it(list);
  while (it.hasNext()) {
    QString each = it.next();
    int offset = 0,
        pos = -1;
    do {
      pos = re.indexIn(each, offset);
      if (pos > -1) {
        each.insert(pos, '\t');
        offset = pos + 2;
      }
    } while(pos > -1);

    result << each.replace("}", "\t}");
  }
  return result;
}

QStringList jsonObjectContent(QStringList::const_iterator& first, const QStringList::const_iterator& last)
{
  QStringList result;
  while (first != last) {
    const QString& each = *first++;
    result << each;
    if (each.contains("}")) {
      break;
    }
  }
  --first;
  return result;
}

QString captureFirst(const QRegExp& re, const QString& str)
{
  if (re.indexIn(str) > -1) {
    return re.cap(1);
  }
  return QString::null;
}

}

namespace metro {

Station::Station() :
  m_id(0),
  m_line(0)
{
}

void Station::setId(quint32 id)
{
  m_id = id;
}

void Station::setLine(quint32 line)
{
  m_line = line;
}

void Station::setName(const QString& name)
{
  m_name = name;
}

void Station::addCrossOver(quint32 stationId, qint32 cost)
{
  m_crossovers[stationId] = cost;
}

void Station::addRailTrack(quint32 stationId, qint32 cost)
{
  m_railtracks[stationId] = cost;
}

void Station::removeCrossOver(quint32 stationId)
{
  m_crossovers.remove(stationId);
}

void Station::removeRailTrack(quint32 stationId)
{
  m_railtracks.remove(stationId);
}

quint32 Station::id() const
{
  return m_id;
}

quint32 Station::line() const
{
  return m_line;
}

const QString& Station::name() const
{
  return m_name;
}

bool Station::isValid() const
{
  bool invalid =    m_id == 0
                 || m_line == 0
                 || m_name.isEmpty();
  return !invalid;
}

QList<quint32> Station::crossOvers() const
{
  return m_crossovers.keys();
}

QList<quint32> Station::railTracks() const
{
  return m_railtracks.keys();
}

qint32 Station::minimumCostTo(quint32 id, qint32 crossoverPenalty, bool* ok) const
{
  if (ok != 0) {
    *ok = false;
  }

  if (id == m_id) {
    if (ok != 0) {
      *ok = true;
    }
    return 0;
  }

  bool hasCrossover = false,
       hasRailtrack = false;

  qint32 crossoverCost = crossOverCostTo(id, &hasCrossover);
  qint32 railtrackCost = railTrackCostTo(id, &hasRailtrack);

  if (hasCrossover && hasRailtrack) {
    if (ok != 0) {
      *ok = true;
    }
    return qMin(crossoverCost + crossoverPenalty, railtrackCost);
  }
  else if (hasCrossover) {
    if (ok != 0) {
      *ok = true;
    }
    return crossoverCost + crossoverPenalty;
  }
  else if (hasRailtrack) {
    if (ok != 0) {
      *ok = true;
    }
    return railtrackCost;
  }
  return 0;
}

qint32 Station::railTrackCostTo(quint32 id, bool* ok) const
{
  if (ok != 0) {
    *ok = false;
  }

  if (id == m_id) {
    if (ok != 0) {
      *ok = true;
    }
    return 0;
  }

  if (m_railtracks.contains(id)) {
    if (ok != 0) {
      *ok = true;
    }
    return m_railtracks.value(id);
  }
  return 0;
}

qint32 Station::crossOverCostTo(quint32 id, bool* ok) const
{
  if (ok != 0) {
    *ok = false;
  }

  if (id == m_id) {
    if (ok != 0) {
      *ok = true;
    }
    return 0;
  }

  if (m_crossovers.contains(id)) {
    if (ok != 0) {
      *ok = true;
    }
    return m_crossovers.value(id);
  }
  return 0;
}

QString Station::toJsonString() const
{
  typedef QMap<quint32, qint32>::const_iterator iter_t;

  QString id   = makeJsonPair(::jsonKeys()[ID],   m_id  );
  QString line = makeJsonPair(::jsonKeys()[LINE], m_line);
  QString name = makeJsonPair(::jsonKeys()[NAME], m_name);

  QStringList tmplist;
  for (iter_t it = m_crossovers.constBegin(), end = m_crossovers.constEnd(); it != end; ++it) {
    tmplist << makeJsonPair(it.key(), it.value());
  }
  QString crossovers = makeJsonObject(::jsonKeys()[CROSSOVER], tmplist);

  tmplist.clear();
  for (iter_t it = m_railtracks.constBegin(), end = m_railtracks.constEnd(); it != end; ++it) {
    tmplist << makeJsonPair(it.key(), it.value());
  }
  QString railtracks = makeJsonObject(::jsonKeys()[RAILTRACK], tmplist);

  tmplist.clear();
  tmplist << id
          << line
          << name
          << railtracks
          << crossovers;

  return makeJsonObject(::jsonKeys()[STATION], tmplist);
}

QString Station::makeJsonObject(const QString& key, const QStringList& values) const
{
  QString jsonkey = !key.isEmpty()    ? QString("\"%1\": ").arg(key)
                                      : QString::null;
  QString jsonval = !values.isEmpty() ? QString("\n\t%1").arg(::addTabLevel(values).join(",\n\t"))
                                      : QString::null;
  return QString("%1{%2\n}").arg(jsonkey).arg(jsonval);
}

Station Station::fromJsonString(const QString& str)
{
  typedef QMap<json_keys_t, QString>::const_iterator iter_t;

  Station result;

  QStringList stationContent = str.split("\n", QString::SkipEmptyParts);
  for (QStringList::const_iterator stationIt = stationContent.constBegin(), stationEnd = stationContent.end(); stationIt != stationEnd; ++stationIt) {
    const QString& each = *stationIt;
    bool founded = false;
    for (iter_t it = ::jsonKeys().constBegin(), end = ::jsonKeys().constEnd(); it != end; ++it) {
      if (each.contains(it.value())) {
        switch (it.key()) {
          case ID : {
              bool ok = false;
              quint32 id = jsonValue(each).toUInt(&ok);
              if (ok) {
                result.setId(id);
              }
              founded = true;
            } break;
          case LINE: {
              bool ok = false;
              quint32 line = jsonValue(each).toUInt(&ok);
              if (ok) {
                result.setLine(line);
              }
              founded = true;
            } break;
          case NAME: {
              result.setName(jsonValue(each));
              founded = true;
            } break;
          case RAILTRACK: {
              result.addFromJsonContent(result.m_railtracks,
                                        ::jsonObjectContent(stationIt, stationEnd));
              founded = true;
            } break;
          case CROSSOVER: {
              result.addFromJsonContent(result.m_crossovers,
                                        ::jsonObjectContent(stationIt, stationEnd));
              founded = true;
            } break;
          default:
              founded = true;
            break;
        }
      }
      if (founded) {
        break;
      }
    }
  }
  return result;
}

void Station::addFromJsonContent(QMap<quint32, qint32>& target, QStringList jsonContent)
{
  jsonContent.removeFirst();
  jsonContent.removeLast();

  QStringListIterator it(jsonContent);
  while (it.hasNext()) {
    const QString& each = it.next();
    bool ok = false;
    quint32 stationId = jsonKey(each).toUInt(&ok);
    if (ok) {
      qint32 cost = jsonValue(each).toInt(&ok);
      if (ok) {
        target.insert(stationId, cost);
      }
    }
  }
}

QString Station::jsonKey(const QString& jsonPair)
{
  QRegExp re("\\\"([^\\\"]*)\\\":\\s*");
  return ::captureFirst(re, jsonPair);
}

QString Station::jsonValue(const QString& jsonPair)
{
  QRegExp re(":\\s*\\\"([^\\\"]*)\\\"");
  return ::captureFirst(re, jsonPair);
}

} // metro

bool operator==(const metro::Station& lhs, const metro::Station& rhs)
{
  return   lhs.id() == rhs.id()
        && lhs.line() == rhs.line()
        && lhs.name() == rhs.name()
        && lhs.railTracks() == rhs.railTracks()
        && lhs.crossOvers() == rhs.crossOvers();
}

QTextStream& operator<<(QTextStream& out, const metro::Station& station)
{
  return out << station.toJsonString();
}

QTextStream& operator>>(QTextStream& in, metro::Station& station)
{
  QString tmp = in.readLine();
  QString stationStart = QString("%1{")
                         .arg((::jsonKeys()[STATION].isEmpty() ? QString::null
                                                               : QString("\"%1\": ")
                                                                 .arg(::jsonKeys()[STATION])));
  while (   !tmp.trimmed().startsWith(stationStart)
         && !in.atEnd()) {
    tmp = in.readLine();
  }

  if (!in.atEnd()) {
    int openedBrackets = 1,
        closedBrackets = 0;

    QStringList content;
    content << tmp;
    while (   openedBrackets > closedBrackets
              && !in.atEnd()) {
      tmp = in.readLine().trimmed();
      if (tmp.contains("{")) {
        ++openedBrackets;
      }
      if (tmp.contains("}")) {
        ++closedBrackets;
      }
      content << tmp;
    }
    if (openedBrackets == closedBrackets) {
      station = metro::Station::fromJsonString(content.join("\n"));
    }
  }
  return in;
}
