#include "mapview.h"
#include "ui_mapview.h"

#include "crossoveritem.h"
#include "metromapmainwindow.h"
#include "railtrackitem.h"
#include "stationitem.h"

#include <exception/exception.h>
#include <map/map.h>
#include <settings/settings.h>
#include <station/station.h>

#include <QAction>
#include <QColor>
#include <QContextMenuEvent>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QLineF>
#include <QList>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>
#include <QPair>
#include <QPointF>

namespace metro {

MapView::MapView(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::MapView()),
  m_controller(ctrl),
  m_mode(SHOW),
  m_hasRoute(false),
  m_from(0),
  m_to(0)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  initScene();

  m_ui->view->viewport()->installEventFilter(this);

  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
  connect(this, SIGNAL(fromSelected(quint32)), SIGNAL(stationSelected(quint32)));
  connect(this, SIGNAL(toSelected(quint32)), SIGNAL(stationSelected(quint32)));
  connect(this, SIGNAL(stationEdited(quint32)), SIGNAL(stationSelected(quint32)));
  connect(this, SIGNAL(stationSelected(quint32)), SLOT(slotSelectStations(quint32)));
  connect(this, SIGNAL(stationDeselected(quint32)), SLOT(slotDeselectStation(quint32)));
}

MapView::~MapView()
{
  delete m_ui;
  m_ui = 0;
}

void MapView::initScene()
{
  QGraphicsScene* scene = new QGraphicsScene(this);
  m_ui->view->setScene(scene);
}

void MapView::slotSelectStations(quint32 id)
{
  slotSelectStations(selectedStations() << id);
}

void MapView::slotSelectStations(const QList<quint32>& stations)
{
  clearSelection();
  QListIterator<quint32> it(stations);
  while (it.hasNext()) {
    quint32 id = it.next();
    StationItem* item = stationItemById(id);
    if (item != 0) {
      item->selectStation(true);
    }
  }
}

void MapView::slotShowRoute(const QList<quint32>& stations)
{
  slotSelectStations(stations);
  transparenceRoute(stations);
  m_hasRoute = true;
}

void MapView::transparenceRoute(const QList<quint32>& route)
{
  if (route.isEmpty()) {
    return;
  }

  const qreal transparentLevel = 1.0;
  const qreal opacityLevel = 0.2;

  foreach (QGraphicsItem* item, m_ui->view->scene()->items()) {
    item->setOpacity(opacityLevel);
  }

  {
    QListIterator<quint32> it(route);
    while (it.hasNext()) {
      StationItem* each = stationItemById(it.next());
      if (each != 0) {
        each->setOpacity(transparentLevel);
      }
    }
  }

  typedef QList<quint32>::const_iterator iter_t;
  for (iter_t it = route.constBegin(), end = route.constEnd(); it != end; ++it) {
    if (it+1 != end) {
      quint32 from = *it;
      quint32 to = *(it+1);
      RailTrackItem* rt = railTrackItemById(from, to);
      if (rt != 0) {
        rt->setOpacity(transparentLevel);
      }
      else {
        CrossOverItem* co = crossOverItemById(from, to);
        if (co != 0) {
          co->setOpacity(transparentLevel);
        }
      }
    }
  }
}

void MapView::slotDeselectStation(quint32 id)
{
  StationItem* item = stationItemById(id);
  if (item != 0) {
    item->selectStation(false);
  }

  if (m_from == id) {
    m_from = 0;
  }
  if (m_to == id) {
    m_to = 0;
  }
}

void MapView::clearSelection()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    each->setOpacity(1.0);
    StationItem* item = qgraphicsitem_cast<StationItem*>(each);
    if (item != 0) {
      item->selectStation(false);
    }
  }
  m_hasRoute = false;
}

QList<quint32> MapView::selectedStations() const
{
  QList<quint32> result;
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = qgraphicsitem_cast<StationItem*>(each);
    if (item != 0 && item->isSelectedStation()) {
      result.append(item->id());
    }
  }
  return result;
}

void MapView::slotMapChanged()
{
  m_ui->view->scene()->clear();

  renderStations();
  renderRailTracks();
  renderCrossOvers();

  switch (m_mode) {
    case SHOW:
        setEnableStationsMoving(false);
      break;
    case EDIT:
        setEnableStationsMoving(true);
      break;
    default:
      break;
  }
}

void MapView::renderStations()
{
  QListIterator<quint32> it(m_controller->map().stationsId());
  while (it.hasNext()) {
    const Station& each = m_controller->map().stationById(it.next());
    StationItem* item = addStation(each.id(),
                                   settings::Loader::getLineColor(each.line()),
                                   settings::Loader::loadStationPosition(each.id()));
    item->setStationName(each.name());
  }
}

StationItem* MapView::stationItemById(quint32 id) const
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = qgraphicsitem_cast<StationItem*>(each);
    if (item != 0 && item->id() == id) {
      return item;
    }
  }
  return 0;
}

RailTrackItem* MapView::railTrackItemById(quint32 from, quint32 to) const
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    RailTrackItem* item = qgraphicsitem_cast<RailTrackItem*>(each);
    if (   item != 0
        && (item->stationFrom() == from && item->stationTo() == to)) {
      return item;
    }
  }
  return 0;
}

CrossOverItem* MapView::crossOverItemById(quint32 from, quint32 to) const
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    CrossOverItem* item = qgraphicsitem_cast<CrossOverItem*>(each);
    if (   item != 0
        && (item->stationFrom() == from && item->stationTo() == to)) {
      return item;
    }
  }
  return 0;
}

void MapView::renderRailTracks()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* itemFrom = qgraphicsitem_cast<StationItem*>(each);
    if (itemFrom != 0 && m_controller->map().containsStation(itemFrom->id())) {
      const Station& st = m_controller->map().stationById(itemFrom->id());
      foreach (quint32 id, st.railTracks()) {
        StationItem* itemTo = stationItemById(id);
        if (itemTo != 0) {
          RailTrackItem* rt = new RailTrackItem(*itemFrom, *itemTo, settings::Loader::getLineColor(st.line()));
          rt->setZValue(settings::Loader::medZValue());
          m_ui->view->scene()->addItem(rt);
        }
      }
    }
  }
}

void MapView::renderCrossOvers()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* itemFrom = qgraphicsitem_cast<StationItem*>(each);
    if (itemFrom != 0 && m_controller->map().containsStation(itemFrom->id())) {
      const Station& st = m_controller->map().stationById(itemFrom->id());
      foreach (quint32 id, st.crossOvers()) {
        StationItem* itemTo = stationItemById(id);
        if (itemTo != 0) {
          CrossOverItem* co = new CrossOverItem(*itemFrom, *itemTo);
          co->setZValue(settings::Loader::minZValue());
          m_ui->view->scene()->addItem(co);
        }
      }
    }
  }
}

bool MapView::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == m_ui->view->viewport()) {
    switch (event->type()) {
      case QEvent::MouseButtonPress: {
          QMouseEvent* me = static_cast<QMouseEvent*>(event);
          if (me->button() == Qt::LeftButton) {
            StationItem* item = qgraphicsitem_cast<StationItem*>(m_ui->view->scene()->itemAt(m_ui->view->mapToScene(me->pos())));
            if (item != 0) {
              selectStation(item);
            }
          }
          else if (me->button() == Qt::RightButton) {
            showContextMenu(me->pos());
          }
        }
        break;
      default:
        break;
    }
  }
  return QWidget::eventFilter(watched, event);
}

void MapView::showContextMenu(const QPoint& pos)
{
  StationItem* item = qgraphicsitem_cast<StationItem*>(m_ui->view->scene()->itemAt(m_ui->view->mapToScene(pos)));

  QMenu m;
  QAction* mode = new QAction(QObject::tr("Edit mode"), &m);
  mode->setCheckable(true);
  m.addAction(mode);
  QAction* add = 0;
  QAction* edit = 0;
  QAction* remove = 0;
  switch (m_mode) {
    case SHOW: {
        mode->setChecked(false);
      }
      break;
    case EDIT: {
        mode->setChecked(true);
        m.addSeparator();
        add = new QAction(QObject::tr("Add station"), &m);
        m.addAction(add);
        if (stationItemById(settings::Loader::maxStationId()) != 0) {
          add->setEnabled(false);
        }
        if (item != 0) {
          edit = new QAction(QObject::tr("Edit station"), &m);
          m.addAction(edit);
          remove = new QAction(QObject::tr("Remove station"), &m);
          m.addAction(remove);
        }
      }
      break;
    default:
      break;
  }

  QAction* answer = m.exec(mapToGlobal(pos));
  if (answer != 0) {
    if (answer == mode) {
      switch(m_mode) {
        case SHOW:
            slotToEditMode();
          break;
        case EDIT:
            slotToShowMode();
          break;
        default:
          break;
      }
    }
    else if (answer == add) {
      quint32 id = settings::Loader::maxStationId();
      while (m_controller->map().containsStation(id)) {
        if (--id == 0) {
          throw Exception(QObject::tr("Overflow limit of stations count"));
        }
      }
      StationItem* st = addStation(id, QColor(Qt::gray), m_ui->view->mapToScene(pos));
      st->selectStation(true);
      emit stationAdded(id);
    }
    else if (answer == edit) {
      selectStation(item);
    }
    else if (answer == remove) {
      quint32 id = item->id();
      delete item;
      emit stationRemoved(id);
    }
  }
}

void MapView::slotToShowMode()
{
  m_mode = SHOW;
  setEnableStationsMoving(false);
}

void MapView::slotToEditMode()
{
  m_mode = EDIT;
  setEnableStationsMoving(true);
}

void MapView::setEnableStationsMoving(bool enable)
{
  foreach (QGraphicsItem* item, m_ui->view->scene()->items()) {
    StationItem* st = qgraphicsitem_cast<StationItem*>(item);
    if (st != 0) {
      st->setFlag(QGraphicsItem::ItemIsMovable, enable);
    }
  }
}

void MapView::selectStation(StationItem* item)
{
  if (item == 0) {
    return;
  }

  switch (m_mode) {
    case SHOW: {
        if (m_hasRoute) {
          clearSelection();
        }
        if (item->isSelectedStation()) {
          emit stationDeselected(item->id());
        }
        else {
          if (m_from == 0) {
            m_from = item->id();
            emit fromSelected(m_from);
          }
          else {
            m_to = item->id();
            emit toSelected(m_to);
          }

          foreach (quint32 id, selectedStations()) {
            if (id == m_to || id == m_from) {
              continue;
            }
            stationDeselected(id);
          }
        }
      }
      break;
    case EDIT:
        clearSelection();
        emit stationEdited(item->id());
      break;
    default:
      break;
  }
}

StationItem* MapView::addStation(quint32 id, const QColor& color, const QPointF& pos)
{
  StationItem* item = new StationItem(id, color);
  item->setZValue(settings::Loader::maxZValue());
  m_ui->view->scene()->addItem(item);

  if (!pos.isNull()) {
    item->setPos(pos);
  }
  else {
    item->setDefaultPos();
  }
  return item;
}

} // metro
