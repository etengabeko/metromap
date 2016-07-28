#ifndef METROMAP_UI_CROSSOVERITEM_H
#define METROMAP_UI_CROSSOVERITEM_H

#include <QGraphicsPathItem>
#include <QPair>
#include <QPointF>

class QPainterPath;

namespace metro {

class StationItem;

class CrossOverItem : public QGraphicsPathItem
{
public:
  explicit CrossOverItem(const StationItem& from, const StationItem& to, QGraphicsItem* parent = 0);
  ~CrossOverItem();

  quint32 stationFrom() const;
  quint32 stationTo() const;

  void setCoordFrom(const QPointF& pos);
  void setCoordTo(const QPointF& pos);

private:
  QPainterPath makePath() const;
  QPainterPath makePath(const QPointF& from, const QPointF& to) const;

private:
  QPair<quint32, QPointF> m_from;
  QPair<quint32, QPointF> m_to;


};

} // metro

#endif // METROMAP_UI_CROSSOVERvvITEM_H
