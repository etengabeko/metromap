#include "crossoveritem.h"

#include "stationitem.h"

#include <settings/settings.h>

#include <QPen>
#include <QPainterPath>

namespace metro {

CrossOverItem::CrossOverItem(const StationItem& from, const StationItem& to, QGraphicsItem* parent) :
  QGraphicsPathItem(makePath(from.coordCenter(), to.coordCenter()), parent),
  m_from(qMakePair(from.id(), from.coordCenter())),
  m_to(qMakePair(to.id(), to.coordCenter()))
{
  QPen p;
  p.setWidth(settings::Loader::getPenWidth()/2);
  setPen(p);
}

CrossOverItem::~CrossOverItem()
{
}

quint32 CrossOverItem::stationFrom() const
{
  return m_from.first;
}

quint32 CrossOverItem::stationTo() const
{
  return m_to.first;
}

void CrossOverItem::setCoordFrom(const QPointF& pos)
{
  m_from.second = pos;
  setPath(makePath());
}

void CrossOverItem::setCoordTo(const QPointF& pos)
{
  m_to.second = pos;
  setPath(makePath());
}

QPainterPath CrossOverItem::makePath() const
{
  return makePath(m_from.second, m_to.second);
}

QPainterPath CrossOverItem::makePath(const QPointF& from, const QPointF& to) const
{
  const qreal radius = settings::Loader::getStationRadius() + 5.0;

  QPolygonF by_x = QPolygonF() << QPointF(from.x(), from.y()-radius)
                               << QPointF(from.x(), from.y()+radius)
                               << QPointF(to.x(), to.y()+radius)
                               << QPointF(to.x(), to.y()-radius);
  by_x.append(by_x.first());

  QPolygonF by_y = QPolygonF() << QPointF(from.x()-radius, from.y())
                               << QPointF(from.x()+radius, from.y())
                               << QPointF(to.x()+radius, to.y())
                               << QPointF(to.x()-radius, to.y());
  by_y.append(by_y.first());

  QPainterPath start;  start.addEllipse(from, radius, radius);
  QPainterPath body_x; body_x.addPolygon(by_x);
  QPainterPath body_y; body_y.addPolygon(by_y);
  QPainterPath end;    end.addEllipse(to, radius, radius);

  QPainterPath result = start.united(body_x);
  result = result.united(body_y);
  result = result.united(end);
  return result;
}

} // metro
