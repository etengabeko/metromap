#include "mapview.h"
#include "ui_mapview.h"

#include "metromapmainwindow.h"

#include <map/map.h>
#include <station/station.h>

#include <QList>
#include <QTime>

namespace metro {

MapView::MapView(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::MapView()),
  m_controller(ctrl)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
}

MapView::~MapView()
{
  delete m_ui;
  m_ui = 0;
}

void MapView::slotSelectStations(const QList<quint32>& stations)
{
  // TODO
  QStringList tmp;

  QListIterator<quint32> it(stations);
  while (it.hasNext()) {
    quint32 id = it.next();
    if (m_controller->map().containsStation(id)) {
      tmp.append(QString("%1").arg(m_controller->map().stationById(id).name()));
    }
  }
  debugString(tmp.join(" => "));
}

void MapView::debugString(const QString& str)
{
  QString content = m_ui->debugTextEdit->toPlainText();
  content += QString("%1: %2\n").arg(QTime::currentTime().toString("hh:mm:ss")).arg(str);
  m_ui->debugTextEdit->setPlainText(content);
}

void MapView::slotMapChanged()
{
  // TODO
  m_ui->debugTextEdit->clear();
}

} // metro
