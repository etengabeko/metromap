#include "mapview.h"
#include "ui_mapview.h"

#include "metromapmainwindow.h"
#include "stationitem.h"

#include <exception/exception.h>
#include <map/map.h>
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
#include <QRegExp>
#include <QSettings>

#include <QDebug>

namespace {

QString settingsFileName() { return QString("%1/../etc/metromap.conf").arg(qApp->applicationDirPath()); }

QColor getLineColor(quint32 line)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  QString colorName = settings.value(QString("line_colors/%1").arg(line), QString("gray")).toString();
  return QColor(colorName);
}

void saveStationPosition(quint32 id, const QPointF& pos)
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  settings.setValue(QString("station_positions/%1").arg(id),
                    QString("(%1,%2)")
                    .arg(QString::number(pos.x(), 'f', 2))
                    .arg(QString::number(pos.y(), 'f', 2)));
}

QPointF loadStationPosition(quint32 id, const QPointF& defaultPos = QPointF())
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);
  if (settings.contains(QString("station_positions/%1").arg(id))) {
    QString str_pos = settings.value(QString("station_positions/%1").arg(id)).toString();
    QRegExp re("\\(([\\d|\\.|-]+),([\\d|\\.|-]+)\\)");
    if (re.indexIn(str_pos) > -1) {
      bool ok = false;
      qreal x = re.cap(1).toDouble(&ok);
      if (ok) {
        qreal y = re.cap(2).toDouble(&ok);
        if (ok) {
          return QPointF(x,y);
        }
      }
    }
  }
  return defaultPos;
}

int getLineWidth() { return 3; }

qreal minZValue() { return -99.0; }
qreal medZValue() { return   0.0; }
//qreal maxZValue() { return  99.0; }

quint32 maxStationId() { return static_cast<quint32>(0xFFFFFFFF); }

}

namespace metro {

MapView::MapView(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::MapView()),
  m_controller(ctrl),
  m_mode(SHOW),
  m_from(0),
  m_to(0)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  m_ui->routeGroupBox->hide();

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
    StationItem* item = itemById(id);
    if (item != 0) {
      item->selectStation(true);
    }
  }
  if (m_ui->routeGroupBox->isVisible()) {
    m_ui->routeGroupBox->hide();
  }
}

void MapView::slotShowRouteInfo(const QList<quint32>& stations)
{
  slotSelectStations(stations);

  if (!m_ui->routeGroupBox->isVisible()) {
    m_ui->routeGroupBox->show();
  }

  QStringList steps;
  qint32 totalCost = 0;
  int railtracks = 0;
  int crossovers = 0;

  if (!stations.isEmpty()) {
    for (QList<quint32>::const_iterator it = stations.constBegin(), end = stations.constEnd(); it != end-1; ++it) {
      if (   m_controller->map().containsStation(*it)
             && m_controller->map().containsStation(*(it+1))) {
        const Station& from = m_controller->map().stationById(*it);
        const Station& to = m_controller->map().stationById(*(it+1));
        bool ok = false;
        qint32 cost = from.minimumCostTo(to.id(), 0, &ok);
        if (ok == true) {
          totalCost += cost;
          steps.append(from.name());
          if (it+2 == end) {
            steps.append(to.name());
          }
        }
        if (from.railTracks().contains(to.id())) {
          ++railtracks;
        }
        else if (from.crossOvers().contains(to.id())) {
          ++crossovers;
        }
      }
    }
  }
  m_ui->stepsLineEdit->setText(steps.join(" => "));
  m_ui->costLineEdit->setText(QString::number(totalCost));
  m_ui->railtracksLineEdit->setText(QString::number(railtracks));
  m_ui->crossoversLineEdit->setText(QString::number(crossovers));
}

void MapView::slotDeselectStation(quint32 id)
{
  StationItem* item = itemById(id);
  if (item != 0) {
    item->selectStation(false);
  }
}

void MapView::clearSelection()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = dynamic_cast<StationItem*>(each);
    if (item != 0) {
      item->selectStation(false);
    }
  }
}

QList<quint32> MapView::selectedStations() const
{
  QList<quint32> result;
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = dynamic_cast<StationItem*>(each);
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
  QMap<quint32, int> stationsOnLines;
  foreach (quint32 each, m_controller->map().linesId()) {
    stationsOnLines.insert(each, 0);
  }

  QPointF defaultPos = QPointF(0.0, 0.0);
  const qreal vstep = StationItem::stationEllipseRadius()*4;
  const qreal hstep = vstep*4;

  QListIterator<quint32> it(m_controller->map().stationsId());
  while (it.hasNext()) {
    const Station& each = m_controller->map().stationById(it.next());
    QPointF eachPos(defaultPos.x() + each.line()*hstep, defaultPos.y() + stationsOnLines[each.line()]*vstep);
    eachPos = ::loadStationPosition(each.id(), eachPos);
    StationItem* item = addStation(each.id(), eachPos, ::getLineColor(each.line()));
    item->setStationName(each.name());
    stationsOnLines[each.line()] += 1;
  }
}

StationItem* MapView::itemById(quint32 id) const
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = dynamic_cast<StationItem*>(each);
    if (item != 0 && item->id() == id) {
      return item;
    }
  }
  return 0;
}

void MapView::renderRailTracks()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = dynamic_cast<StationItem*>(each);
    if (item != 0 && m_controller->map().containsStation(item->id())) {
      const Station& st = m_controller->map().stationById(item->id());
      foreach (quint32 id, st.railTracks()) {
        StationItem* to = itemById(id);
        if (to != 0) {
          QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(item->coordCenter(), to->coordCenter()));
          QPen p;
          p.setWidth(::getLineWidth());
          p.setColor(::getLineColor(st.line()));
          line->setPen(p);
          line->setZValue(::minZValue());
          m_ui->view->scene()->addItem(line);
        }
      }
    }
  }
}

void MapView::renderCrossOvers()
{
  foreach (QGraphicsItem* each, m_ui->view->scene()->items()) {
    StationItem* item = dynamic_cast<StationItem*>(each);
    if (item != 0 && m_controller->map().containsStation(item->id())) {
      const Station& st = m_controller->map().stationById(item->id());
      foreach (quint32 id, st.crossOvers()) {
        StationItem* to = itemById(id);
        if (to != 0) {
          QGraphicsLineItem* line = new QGraphicsLineItem(QLineF(item->coordCenter(), to->coordCenter()));
          QPen p;
          p.setStyle(Qt::DotLine);
          line->setPen(p);
          line->setZValue(::minZValue());
          m_ui->view->scene()->addItem(line);
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
          switch (m_mode) {
            case SHOW:
                if (me->button() == Qt::LeftButton) {
                  StationItem* item = dynamic_cast<StationItem*>(m_ui->view->scene()->itemAt(m_ui->view->mapToScene(me->pos())));
                  if (item != 0) {
                    selectStation(item, me->pos());
                  }
                }
                else if (me->button() == Qt::RightButton) {
                  showContextMenu(me->pos());
                }
              break;
            case EDIT:
                if (me->button() == Qt::RightButton) {
                  showContextMenu(me->pos());
                }
              break;
            default:
              break;
          }
        }
        break;
      case QEvent::MouseButtonRelease: {
          QMouseEvent* me = static_cast<QMouseEvent*>(event);
          switch (m_mode) {
            case SHOW:
              break;
            case EDIT:
              if (me->button() == Qt::LeftButton) {
                saveItemPos(me->pos());
                showContextMenu(me->pos());
              }
              break;
            default:
              break;
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
  StationItem* item = dynamic_cast<StationItem*>(m_ui->view->scene()->itemAt(m_ui->view->mapToScene(pos)));

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
        if (itemById(::maxStationId()) != 0) {
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
      quint32 id = ::maxStationId();
      while (m_controller->map().containsStation(id)) {
        if (--id == 0) {
          throw Exception(QObject::tr("Overflow limit of stations count"));
        }
      }
      StationItem* st = addStation(id, m_ui->view->mapToScene(pos));
      st->selectStation(true);
      emit stationAdded(id);
    }
    else if (answer == edit) {
      selectStation(item, pos);
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
    StationItem* st = dynamic_cast<StationItem*>(item);
    if (st != 0) {
      st->setFlag(QGraphicsItem::ItemIsMovable, enable);
    }
  }
}

void MapView::selectStation(StationItem* item, const QPoint& pos)
{
  if (item == 0) {
    return;
  }

  switch (m_mode) {
    case SHOW: {
        if (item->isSelectedStation()) {
          emit stationDeselected(item->id());
        }
        else {
          QMenu m;
          QAction* from = new QAction(QObject::tr("From"), &m);
          QAction* to = new QAction(QObject::tr("To"), &m);
          m.addAction(from);
          m.addAction(to);
          QAction* answer = m.exec(mapToGlobal(pos));
          if (answer == from) {
            emit stationDeselected(m_from);
            m_from = item->id();
            emit fromSelected(m_from);
          }
          else if (answer == to) {
            emit stationDeselected(m_to);
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

StationItem* MapView::addStation(quint32 id, const QPointF& pos, const QColor& color)
{
  StationItem* item = new StationItem(id, pos, color);
  item->setZValue(::medZValue());
  m_ui->view->scene()->addItem(item);
  return item;
}

void MapView::saveItemPos(const QPoint& pos)
{
  QPointF scenePos = m_ui->view->mapToScene(pos);
  StationItem* item = dynamic_cast<StationItem*>(m_ui->view->scene()->itemAt(scenePos));
  if (   item != 0
      && item->id() > 0
      && item->id() < ::maxStationId()) {
    ::saveStationPosition(item->id(), scenePos);
  }
}

} // metro
