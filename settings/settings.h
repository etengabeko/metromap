#ifndef METROMAP_SETTINGS_SETTINGS_H
#define METROMAP_SETTINGS_SETTINGS_H

#include <QtGlobal>
#include <QPointF>

class QColor;
class QString;

namespace metro {
namespace settings {

class Loader
{
public:
  static QString settingsFileName();

  static QColor getLineColor(quint32 line);
  static int getPenWidth();

  static qreal getStationRadius();

  static qreal minZValue();
  static qreal medZValue();
  static qreal maxZValue();

  static quint32 maxStationId();

  static void saveStationPosition(quint32 id, const QPointF& pos);
  static QPointF loadStationPosition(quint32 id, const QPointF& defaultPos = QPointF());

};

} // settings
} //metro

#endif // METROMAP_SETTINGS_SETTINGS_H
