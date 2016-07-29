#ifndef METROMAP_SETTINGS_SETTINGS_H
#define METROMAP_SETTINGS_SETTINGS_H

#include <QtGlobal>
#include <QPointF>
#include <QString>

class QColor;

namespace metro {
namespace settings {

class Loader
{
public:
  static void init(const QString& mapFileName);

  static QString settingsFileName();

  static QColor getLineColor(quint32 line);
  static void setLineColor(quint32 line, const QColor& color);

  static int getPenWidth();

  static qreal getStationRadius();

  static qreal minZValue();
  static qreal medZValue();
  static qreal maxZValue();

  static quint32 maxStationId();

  static void saveStationPosition(quint32 id, const QPointF& pos);
  static QPointF loadStationPosition(quint32 id, const QPointF& defaultPos = QPointF());

private:
  static QString m_fileName;
  static bool m_isTemp;

};

} // settings
} //metro

#endif // METROMAP_SETTINGS_SETTINGS_H
