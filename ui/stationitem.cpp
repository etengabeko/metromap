#include "stationitem.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QRectF>
#include <QString>

namespace {

qreal stationEllipseRadius() { return 15.0; }

}

namespace metro {

StationItem::StationItem(quint32 id, const QPointF& topleft, QGraphicsItem* parent) :
  QGraphicsEllipseItem(StationItem::stationRect(topleft), parent),
  m_id(id),
  m_name(new QGraphicsTextItem(this))
{
  m_name->setPos(QPointF(pos().x() + ::stationEllipseRadius()*2, pos().y()));
//  QPen p = pen();
//  p.setColor(Qt::black);
//  setPen(p);
}

StationItem::~StationItem()
{
}

quint32 StationItem::id() const
{
  return m_id;
}

QRectF StationItem::stationRect(const QPointF& topleft)
{
  return QRectF(topleft, QSizeF(::stationEllipseRadius()*2, ::stationEllipseRadius()*2));
}

void StationItem::setStationName(const QString& name)
{
  m_name->setPlainText(name);
}

}
