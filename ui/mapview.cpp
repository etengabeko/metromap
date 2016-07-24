#include "mapview.h"
#include "ui_mapview.h"

#include "metromapmainwindow.h"
#include "stationitem.h"

#include <map/map.h>
#include <station/station.h>

#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QLabel>
#include <QList>
#include <QPointF>

#include <QDebug>

namespace metro {

MapView::MapView(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::MapView()),
  m_controller(ctrl)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  initScene();

  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
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
  slotSelectStations(QList<quint32>() << id);
}

void MapView::slotSelectStations(const QList<quint32>& stations)
{
  // TODO
  QListIterator<quint32> it(stations);
  qreal vstep = 30.0;
  QPointF pos = QPointF(0.0, 0.0);
  while (it.hasNext()) {
    quint32 id = it.next();
    if (m_controller->map().containsStation(id)) {
      StationItem* item = new StationItem(id, pos);
      item->setStationName(m_controller->map().stationById(id).name());
      m_ui->view->scene()->addItem(item);
      pos.setY(pos.y() + vstep);
    }
  }
}

void MapView::slotMapChanged()
{
  // TODO

}

} // metro
