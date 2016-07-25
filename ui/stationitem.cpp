#include "stationitem.h"

#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QRectF>
#include <QString>

namespace {

QBrush selectedBrush() { return QBrush(QColor(Qt::yellow), Qt::SolidPattern); }
QBrush unselectedBrush() { return QBrush(QColor(Qt::white), Qt::SolidPattern); }

}

namespace metro {

StationItem::StationItem(quint32 id, const QPointF& topleft, QGraphicsItem* parent) :
  QGraphicsEllipseItem(stationRect(topleft), parent),
  m_id(id),
  m_name(new QGraphicsTextItem(this))
{
  QPointF namePos(topleft.x() + stationEllipseRadius()*2, topleft.y());
  m_name->setPos(namePos);
}

StationItem::~StationItem()
{
}

quint32 StationItem::id() const
{
  return m_id;
}

qreal StationItem::stationEllipseRadius()
{
  return 15.0;
}

QRectF StationItem::stationRect(const QPointF& topleft)
{
  return QRectF(topleft, QSizeF(stationEllipseRadius()*2, stationEllipseRadius()*2));
}

void StationItem::setStationName(const QString& name)
{
  m_name->setPlainText(name);
}

void StationItem::selectStation(bool selected)
{
  if (selected) {
    setBrush(::selectedBrush());
  }
  else {
    setBrush(::unselectedBrush());
  }
}

bool StationItem::isSelectedStation() const
{
  return brush() == ::selectedBrush();
}

QPointF StationItem::coordTop() const
{
  return (QPointF(scenePos().x() + stationEllipseRadius(),
                  scenePos().y()));
}

QPointF StationItem::coordBottom() const
{
  return (QPointF(scenePos().x() + stationEllipseRadius(),
                  scenePos().y() + 2*stationEllipseRadius()));
}

QPointF StationItem::coordLeft() const
{
  return (QPointF(scenePos().x(),
                  scenePos().y() + stationEllipseRadius()));
}

QPointF StationItem::coordRight() const
{
  return (QPointF(scenePos().x() + 2*stationEllipseRadius(),
                  scenePos().y() + stationEllipseRadius()));
}

}
