#include "routeswidget.h"
#include "ui_routeswidget.h"

#include "metromapmainwindow.h"
#include "selectstationwidget.h"

#include <map/map.h>

namespace metro {

RoutesWidget::RoutesWidget(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::RoutesWidget()),
  m_controller(ctrl)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  QLayoutItem* spacer = m_ui->stationsGroupBox->layout()->takeAt(m_ui->stationsGroupBox->layout()->count()-1);

  SelectStationWidget* st = new SelectStationWidget(this);
  st->setObjectName("from");
  st->setPlaceholderText(QObject::tr("From"));
  m_ui->stationsGroupBox->layout()->addWidget(st);
  st = new SelectStationWidget(this);
  st->setObjectName("to");
  st->setPlaceholderText(QObject::tr("To"));
  m_ui->stationsGroupBox->layout()->addWidget(st);

  m_ui->stationsGroupBox->layout()->addItem(spacer);

  connect(m_ui->calcButton, SIGNAL(clicked()), SLOT(slotCalcRoute()));
  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
}

RoutesWidget::~RoutesWidget()
{
  delete m_ui;
  m_ui = 0;
}

void RoutesWidget::slotCalcRoute()
{
  quint32 from = 0,
          to = 0;
  QList<quint32> mediates;
  foreach (SelectStationWidget* each, m_ui->stationsGroupBox->findChildren<SelectStationWidget*>()) {
    if (each != 0) {
      if (each->objectName() == "from") {
        from = each->selectedStation();
      }
      else if (each->objectName() == "to") {
        to = each->selectedStation();
      }
      else {
        if (each->selectedStation() > 0) {
          mediates.append(each->selectedStation());
        }
      }
    }
  }

  if (from > 0) {
    mediates.push_front(from);
  }
  if (to > 0) {
    mediates.push_back(to);
  }

  QList<quint32> route;
  if (mediates.size() > 1) {
    for (QList<quint32>::const_iterator it = mediates.constBegin(), end = mediates.constEnd(); it != end-1; ++it) {
      from = *it;
      to = *(it+1);
      if (isMinimizeCrossovers()) {
        route.append(m_controller->map().findCrossOverOptimizedPath(from, to));
      }
      else {
        route.append(m_controller->map().findTimeOptimizedPath(from, to));
      }
    }
  }
  emit routeCreated(route);
}

void RoutesWidget::slotMapChanged()
{
  m_stations = SelectStationWidget::getStationsByLines(m_controller->map());

  foreach (SelectStationWidget* st, m_ui->stationsGroupBox->findChildren<SelectStationWidget*>()) {
    if (st != 0) {
      st->setStations(&m_stations);
    }
  }
}

bool RoutesWidget::isMinimizeCrossovers() const
{
  return m_ui->crossoversCheckBox->isChecked();
}

} // metro
