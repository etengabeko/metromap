#include "station.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>

namespace {

typedef QMap<quint32, qint32>::const_iterator IdCostIterator;

enum json_keys_t
{
  STATION,
  ID,
  LINE,
  NAME,
  PREVIOUS,
  NEXT,
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
    jsonKeys.insert(PREVIOUS,  "previous" );
    jsonKeys.insert(NEXT,      "next"     );
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
  qDebug() << QString("Invalid json pair: %1").arg(str);
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

void Station::addNextRailTrack(quint32 stationId, qint32 cost)
{
  m_railtracksNext[stationId] = cost;
}

void Station::addPreviousRailTrack(quint32 stationId, qint32 cost)
{
  m_railtracksPrevious[stationId] = cost;
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

QString Station::toJsonString() const
{
  QString id   = makeJsonPair(::jsonKeys()[ID],   m_id  );
  QString line = makeJsonPair(::jsonKeys()[LINE], m_line);
  QString name = makeJsonPair(::jsonKeys()[NAME], m_name);

  QStringList tmplist;
  for (IdCostIterator it = m_crossovers.constBegin(), end = m_crossovers.constEnd(); it != end; ++it) {
    tmplist << makeJsonPair(it.key(), it.value());
  }
  QString crossovers = makeJsonObject(::jsonKeys()[CROSSOVER], tmplist);

  tmplist.clear();
  for (IdCostIterator it = m_railtracksNext.constBegin(), end = m_railtracksNext.constEnd(); it != end; ++it) {
    tmplist << makeJsonPair(it.key(), it.value());
  }
  QString next = makeJsonObject(::jsonKeys()[NEXT], tmplist);

  tmplist.clear();
  for (IdCostIterator it = m_railtracksPrevious.constBegin(), end = m_railtracksPrevious.constEnd(); it != end; ++it) {
    tmplist << makeJsonPair(it.key(), it.value());
  }
  QString previous = makeJsonObject(::jsonKeys()[PREVIOUS], tmplist);

  tmplist.clear();
  tmplist << id
          << line
          << name
          << previous
          << next
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
  typedef QMap<json_keys_t, QString>::const_iterator Iter;

  Station result;

  QStringList stationContent = str.split("\n", QString::SkipEmptyParts);
  for (QStringList::const_iterator stationIt = stationContent.constBegin(), stationEnd = stationContent.end(); stationIt != stationEnd; ++stationIt) {
    const QString& each = *stationIt;
    bool founded = false;
    for (Iter it = ::jsonKeys().constBegin(), end = ::jsonKeys().constEnd(); it != end; ++it) {
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
          case PREVIOUS: {
              result.addFromJsonContent(result.m_railtracksPrevious,
                                        ::jsonObjectContent(stationIt, stationEnd));
              founded = true;
            } break;
          case NEXT: {
              result.addFromJsonContent(result.m_railtracksNext,
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

} // metromap

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
  if (tmp.trimmed().startsWith(stationStart)) {
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
