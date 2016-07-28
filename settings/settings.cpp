#include "settings.h"

#include <QColor>
#include <QCoreApplication>
#include <QRegExp>
#include <QSettings>
#include <QString>

namespace metro {
namespace settings {

QString Loader::settingsFileName()
{
  return QString("%1/../etc/metromap.conf")
         .arg(qApp->applicationDirPath());
}

QColor Loader::getLineColor(quint32 line)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  QString colorName = settings.value(QString("line_colors/%1").arg(line), QString("gray")).toString();
  return QColor(colorName);
}

int Loader::getPenWidth()
{
  return 5;
}

qreal Loader::getStationRadius()
{
  return 15.0;
}

qreal Loader::minZValue()
{
  return -99.0;
}

qreal Loader::medZValue()
{
  return   0.0;
}

qreal Loader::maxZValue()
{
  return  99.0;
}

quint32 Loader::maxStationId()
{
  return static_cast<quint32>(0xFFFFFFFF);
}

void Loader::saveStationPosition(quint32 id, const QPointF& pos)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  settings.setValue(QString("station_positions/%1").arg(id),
                    QString("(%1,%2)")
                    .arg(QString::number(pos.x(), 'f', 0))
                    .arg(QString::number(pos.y(), 'f', 0)));
}

QPointF Loader::loadStationPosition(quint32 id, const QPointF& defaultPos)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  if (settings.contains(QString("station_positions/%1").arg(id))) {
    QString str_pos = settings.value(QString("station_positions/%1").arg(id)).toString();
    QRegExp re("\\(([\\d|\\.|-]+),([\\d|\\.|-]+)\\)");
    if (re.indexIn(str_pos) > -1) {
      bool ok = false;
      qreal x = re.cap(1).toDouble(&ok);
      if (ok) {
        qreal y = re.cap(2).toDouble(&ok);
        if (ok) {
          return QPointF(x,y);
        }
      }
    }
  }
  return defaultPos;
}

} // settings
} // metro
