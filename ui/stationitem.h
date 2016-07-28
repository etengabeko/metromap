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

  static QRectF stationRect(const QPointF& topleft);

private:
  quint32 m_id;
  QColor m_color;
  QGraphicsTextItem* m_name;

};

class LabelItem : public QGraphicsRectItem
{
public:
  explicit LabelItem(const QPointF& topleft, QGraphicsItem* parent = 0);
  ~LabelItem();

  static QRectF labelRect(const QPointF& topleft);

private:
  QGraphicsTextItem* m_text;

};

} // metro

#endif // METROMAP_UI_STATIONITEM_H
