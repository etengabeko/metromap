#include "settings.h"

#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QTemporaryFile>

namespace {

QString temporaryName()
{
  QTemporaryFile tmp;
  tmp.open();
  return tmp.fileName();
}

}

QString metro::settings::Loader::m_fileName = ::temporaryName();
bool metro::settings::Loader::m_isTemp = true;

namespace metro {
namespace settings {

void Loader::init(const QString& mapFileName)
{
  if (!mapFileName.isEmpty()) {
    QFileInfo map(mapFileName);
    QFileInfo conf(QString("%1/../etc/%2.conf")
                   .arg(qApp->applicationDirPath())
                   .arg(map.fileName()));
    if (m_isTemp) {
      if (!conf.absoluteDir().exists()) {
        conf.absoluteDir().mkpath(conf.absolutePath());
      }
      QFile::rename(m_fileName, conf.absoluteFilePath());
    }
    m_fileName = conf.absoluteFilePath();
    m_isTemp = false;
  }
  else {
    m_fileName = ::temporaryName();
    m_isTemp = true;
  }
}

QString Loader::settingsFileName()
{
  return m_fileName;
}

QColor Loader::getLineColor(quint32 line)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  QString colorName = settings.value(QString("line_colors/%1").arg(line), QString("gray")).toString();
  return QColor(colorName);
}

void Loader::setLineColor(quint32 line, const QColor& color)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  settings.setValue(QString("line_colors/%1").arg(line), color.name());
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
