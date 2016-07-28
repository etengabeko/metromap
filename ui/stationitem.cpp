#include "stationitem.h"

#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QObject>
#include <QPen>
#include <QRectF>
#include <QString>

namespace {

QBrush selectedBrush()
{
  return QBrush(QColor(Qt::yellow), Qt::SolidPattern);
}

QBrush unselectedBrush(const QColor& color)
{
  return QBrush(color, Qt::SolidPattern);
}

int getLineWidth() { return 3; }

}

namespace metro {

StationItem::StationItem(quint32 id, const QPointF& topleft, const QColor& color, QGraphicsItem* parent) :
  QGraphicsEllipseItem(stationRect(topleft), parent),
  m_id(id),
  m_color(color),
  m_name(new QGraphicsTextItem(this))
{
  QPen p;
  p.setColor(m_color);
  p.setWidth(::getLineWidth());
  setPen(p);
  setBrush(::unselectedBrush(m_color));

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
    setBrush(::unselectedBrush(m_color));
  }
}

bool StationItem::isSelectedStation() const
{
  return brush() == ::selectedBrush();
}

QPointF StationItem::coordCenter() const
{
  return boundingRect().center();
}

QPointF StationItem::coordTop() const
{
  QPointF topleft = boundingRect().topLeft();
  return (QPointF(topleft.x() + stationEllipseRadius(),
                  topleft.y()));
}

QPointF StationItem::coordBottom() const
{
  QPointF topleft = boundingRect().topLeft();
  return (QPointF(topleft.x() + stationEllipseRadius(),
                  topleft.y() + 2*stationEllipseRadius()));
}

QPointF StationItem::coordLeft() const
{
  QPointF topleft = boundingRect().topLeft();
  return (QPointF(topleft.x(),
                  topleft.y() + stationEllipseRadius()));
}

QPointF StationItem::coordRight() const
{
  QPointF topleft = boundingRect().topLeft();
  return (QPointF(topleft.x() + 2*stationEllipseRadius(),
                  topleft.y() + stationEllipseRadius()));
}

LabelItem::LabelItem(const QPointF& topleft, QGraphicsItem* parent) :
  QGraphicsRectItem(labelRect(topleft), parent),
  m_text(new QGraphicsTextItem(QObject::tr("Edit Mode"), this))
{
  QPointF textPos(topleft.x()+5.0, topleft.y()+5.0);
  m_text->setPos(textPos);
}

LabelItem::~LabelItem()
{
}

QRectF LabelItem::labelRect(const QPointF& topleft)
{
  const qreal w = 100.0,
              h = 30.0;
  return QRectF(topleft, QSizeF(w,h));
}

}
