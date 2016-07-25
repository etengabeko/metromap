#ifndef METROMAP_UI_STATIONITEM_H
#define METROMAP_UI_STATIONITEM_H

#include <QGraphicsEllipseItem>

class QGraphicsTextItem;
class QPointF;
class QRectF;
class QString;

namespace metro {

class StationItem : public QGraphicsEllipseItem
{
public:
  explicit StationItem(quint32 id, const QPointF& topleft, QGraphicsItem* parent = 0);
  ~StationItem();

  void setStationName(const QString& name);

  quint32 id() const;

  void selectStation(bool selected);
  bool isSelectedStation() const;

  QPointF coordTop() const;
  QPointF coordBottom() const;
  QPointF coordLeft() const;
  QPointF coordRight() const;

  static qreal stationEllipseRadius();
  static QRectF stationRect(const QPointF& topleft);

private:
  quint32 m_id;
  QGraphicsTextItem* m_name;

};

} // metro

#endif // METROMAP_UI_STATIONITEM_H
