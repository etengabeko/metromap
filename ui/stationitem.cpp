#include "stationitem.h"

#include "crossoveritem.h"
#include "railtrackitem.h"

#include <settings/settings.h>

#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
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

}

namespace metro {

StationItem::StationItem(quint32 id, const QColor& color, QGraphicsItem* parent) :
  QGraphicsEllipseItem(parent),
  m_id(id),
  m_color(color),
  m_name(new QGraphicsTextItem(this))
{
  QPen p;
  p.setColor(m_color);
  p.setWidth(settings::Loader::getPenWidth());
  setPen(p);
  setBrush(::unselectedBrush(m_color));

  setRect(stationRect(boundingRect().topLeft()));
  m_name->setPos(mapFromScene(coordRight()));
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
  return settings::Loader::getStationRadius();
}

QRectF StationItem::stationRect(const QPointF& topleft)
{
  return QRectF(topleft, QSizeF(stationEllipseRadius()*2, stationEllipseRadius()*2));
}

void StationItem::setStationName(const QString& name)
{
  m_name->setPlainText(name);
}

void StationItem::setDefaultPos()
{
  QPointF defaultPos = QPointF(0.0, 0.0);
  const qreal vstep = stationEllipseRadius()*4;
  const qreal hstep = vstep*4;

  defaultPos = QPointF(defaultPos.x() + (m_id/10)*hstep,
                       defaultPos.y() + (m_id%10)*vstep);
  setPos(defaultPos);
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
  return mapToScene(boundingRect().center());
}

QPointF StationItem::coordTop() const
{
  QPointF topleft = boundingRect().topLeft();
  return mapToScene(QPointF(topleft.x() + stationEllipseRadius(),
                             topleft.y()));
}

QPointF StationItem::coordBottom() const
{
  QPointF topleft = boundingRect().topLeft();
  return mapToScene(QPointF(topleft.x() + stationEllipseRadius(),
                            topleft.y() + 2*stationEllipseRadius()));
}

QPointF StationItem::coordLeft() const
{
  QPointF topleft = boundingRect().topLeft();
  return mapToScene(QPointF(topleft.x(),
                            topleft.y() + stationEllipseRadius()));
}

QPointF StationItem::coordRight() const
{
  QPointF topleft = boundingRect().topLeft();
  return mapToScene(QPointF(topleft.x() + 2*stationEllipseRadius(),
                            topleft.y() + stationEllipseRadius()));
}

void StationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (   m_id > 0
      && m_id < settings::Loader::maxStationId()) {
    settings::Loader::saveStationPosition(m_id, mapToScene(boundingRect().topLeft()));
    updateRailTracks();
    updateCrossOvers();
  }
  QGraphicsEllipseItem::mouseMoveEvent(event);
}

void StationItem::updateRailTracks()
{
  if (scene() != 0) {
    foreach (QGraphicsItem* each, scene()->items()) {
      RailTrackItem* track = qgraphicsitem_cast<RailTrackItem*>(each);
      if (track != 0) {
        if (track->stationFrom() == m_id) {
          track->setCoordFrom(coordCenter());
        }
        else if (track->stationTo() == m_id) {
          track->setCoordTo(coordCenter());
        }
      }
    }
  }
}

void StationItem::updateCrossOvers()
{
  if (scene() != 0) {
    foreach (QGraphicsItem* each, scene()->items()) {
      CrossOverItem* cross = qgraphicsitem_cast<CrossOverItem*>(each);
      if (cross != 0) {
        if (cross->stationFrom() == m_id) {
          cross->setCoordFrom(coordCenter());
        }
        else if (cross->stationTo() == m_id) {
          cross->setCoordTo(coordCenter());
        }
      }
    }
  }
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
