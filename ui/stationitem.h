#ifndef METROMAP_UI_STATIONITEM_H
#define METROMAP_UI_STATIONITEM_H

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

class QGraphicsTextItem;
class QPointF;
class QRectF;
class QString;

namespace metro {

class StationItem : public QGraphicsEllipseItem
{
public:
  explicit StationItem(quint32 id, const QColor& color, QGraphicsItem* parent = 0);
  ~StationItem();

  void setStationName(const QString& name);
  void setDefaultPos();

  quint32 id() const;

  void selectStation(bool selected);
  bool isSelectedStation() const;

  QPointF coordCenter() const;
  QPointF coordTop() const;
  QPointF coordBottom() const;
  QPointF coordLeft() const;
  QPointF coordRight() const;

  static qreal stationEllipseRadius();

private:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

  void updateRailTracks();
  void updateCrossOvers();

  static QRectF stationRect(const QPointF& topleft);

private:
  quint32 m_id;
  QColor m_color;
  QGraphicsTextItem* m_name;

};

} // metro

#endif // METROMAP_UI_STATIONITEM_H
