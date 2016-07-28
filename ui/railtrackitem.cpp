#include "railtrackitem.h"

#include "stationitem.h"

#include <settings/settings.h>

#include <QColor>
#include <QLineF>
#include <QPen>
#include <QPointF>

namespace metro {

RailTrackItem::RailTrackItem(const StationItem& from, const StationItem& to, const QColor& color, QGraphicsItem* parent) :
  QGraphicsLineItem(makeLine(from.coordCenter(), to.coordCenter()), parent),
  m_from(from.id()),
  m_to(to.id())
{
  QPen p;
  p.setColor(color);
  p.setWidth(settings::Loader::getPenWidth());
  setPen(p);
}

RailTrackItem::~RailTrackItem()
{
}

quint32 RailTrackItem::stationFrom() const
{
  return m_from;
}

quint32 RailTrackItem::stationTo() const
{
  return m_to;
}

void RailTrackItem::setCoordFrom(const QPointF& pos)
{
  setLine(makeLine(pos, line().p2()));
}

void RailTrackItem::setCoordTo(const QPointF& pos)
{
  setLine(makeLine(line().p1(), pos));
}

QLineF RailTrackItem::makeLine(const QPointF& from, const QPointF& to) const
{
  return QLineF(from, to);
}

} // metro
