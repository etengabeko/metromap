#include "findroutes.h"
#include "ui_findroutes.h"

#include "managebuttons.h"
#include "metromapmainwindow.h"
#include "selectstation.h"

#include <map/map.h>

#include <QToolButton>

namespace metro {

FindRoutesWidget::FindRoutesWidget(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::FindRoutesWidget()),
  m_controller(ctrl)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  init();

  connect(m_ui->calcButton, SIGNAL(clicked()), SLOT(slotCalcRoute()));
  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
}

FindRoutesWidget::~FindRoutesWidget()
{
  delete m_ui;
  m_ui = 0;
}

void FindRoutesWidget::init()
{
  SelectStationWidget* from = new SelectStationWidget(this);
  from->setObjectName("from");
  from->setPlaceholderText(QObject::tr("From"));
  connect(from, SIGNAL(stationSelected(quint32)), this, SIGNAL(stationSelected(quint32)));

  SelectStationWidget* to = new SelectStationWidget(this);
  to->setObjectName("to");
  to->setPlaceholderText(QObject::tr("To"));
  connect(to, SIGNAL(stationSelected(quint32)), this, SIGNAL(stationSelected(quint32)));

  ManageButtons* mb = new ManageButtons(this);
  connect(mb, SIGNAL(addClicked()), SLOT(slotAddMediateStation()));
  mb->setManagedWidgets(from, to);

  QLayoutItem* spacer = m_ui->stationsGroupBox->layout()->takeAt(m_ui->stationsGroupBox->layout()->count()-1);
  m_ui->stationsGroupBox->layout()->addWidget(from);
  m_ui->stationsGroupBox->layout()->addWidget(mb);
  m_ui->stationsGroupBox->layout()->addWidget(to);
  m_ui->stationsGroupBox->layout()->addItem(spacer);
}

void FindRoutesWidget::slotAddMediateStation()
{
  ManageButtons* senderMb = qobject_cast<ManageButtons*>(sender());
  if (senderMb == 0) {
    return;
  }

  SelectStationWidget* toSw = senderMb->below();

  SelectStationWidget* newSw = new SelectStationWidget(this);
  newSw->setPlaceholderText(QObject::tr("Through"));
  connect(newSw, SIGNAL(stationSelected(quint32)), this, SIGNAL(stationSelected(quint32)));
  newSw->setStations(&m_stations);

  ManageButtons* newMb = new ManageButtons(newSw);
  connect(newMb, SIGNAL(addClicked()), SLOT(slotAddMediateStation()));
  senderMb->setManagedWidgets(senderMb->above(), newSw);
  newMb->setManagedWidgets(newSw, toSw);

  QToolButton* rmButton = new QToolButton(this);
  connect(newSw, SIGNAL(destroyed()), rmButton, SLOT(deleteLater()));
  rmButton->setMinimumSize(32,32);
  rmButton->setText("−");
  connect(rmButton, SIGNAL(clicked()), newMb, SIGNAL(removeClicked()));
  connect(newMb, SIGNAL(removeClicked()), SLOT(slotRemoveMediateStation()));

  QWidget* wgt = new QWidget(this);
  connect(newSw, SIGNAL(destroyed()), wgt, SLOT(deleteLater()));
  wgt->setLayout(new QHBoxLayout(wgt));
  wgt->layout()->addWidget(newSw);
  wgt->layout()->addWidget(rmButton);


  QBoxLayout* lo = qobject_cast<QBoxLayout*>(m_ui->stationsGroupBox->layout());
  if (lo != 0) {
    int index = lo->indexOf(senderMb);
    lo->insertWidget(++index, wgt);
    lo->insertWidget(++index, newMb);
  }
}

void FindRoutesWidget::slotRemoveMediateStation()
{
  ManageButtons* rmMb = qobject_cast<ManageButtons*>(sender());
  if (rmMb != 0) {
    SelectStationWidget* rmSw = rmMb->above();
    int index = m_ui->stationsGroupBox->layout()->indexOf(rmMb);
    ManageButtons* aboveMb = qobject_cast<ManageButtons*>(m_ui->stationsGroupBox->layout()->itemAt(index-2)->widget());
    if (aboveMb != 0) {
      aboveMb->setManagedWidgets(aboveMb->above(), rmMb->below());
    }
    delete rmSw;
  }
  delete rmMb;
}

void FindRoutesWidget::slotCalcRoute()
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

void FindRoutesWidget::slotMapChanged()
{
  m_stations = SelectStationWidget::getStationsByLines(m_controller->map());

  foreach (SelectStationWidget* st, m_ui->stationsGroupBox->findChildren<SelectStationWidget*>()) {
    if (st != 0) {
      st->setStations(&m_stations);
    }
  }
}

bool FindRoutesWidget::isMinimizeCrossovers() const
{
  return m_ui->crossoversCheckBox->isChecked();
}

} // metro
