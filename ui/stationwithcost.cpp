#include "stationwithcost.h"
#include "ui_stationwithcost.h"

#include "selectstation.h"

namespace metro {

StationWithCost::StationWithCost(QMultiMap<quint32, QPair<quint32, QString> >* stations, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::StationWithCost())
{
  m_ui->setupUi(this);

  m_ui->stationWidget->setStations(stations);

  connect(m_ui->removeButton, SIGNAL(clicked()), SIGNAL(removeClicked()));
}

StationWithCost::~StationWithCost()
{
  delete m_ui;
  m_ui = 0;
}

void StationWithCost::setStation(quint32 id)
{
  m_ui->stationWidget->slotSelectStation(id);
}

void StationWithCost::setCost(qint32 cost)
{
  m_ui->costSpinBox->setValue(cost);
}

void StationWithCost::setReadOnly(bool readOnly)
{
  m_ui->costSpinBox->setReadOnly(readOnly);
  m_ui->removeButton->setEnabled(!readOnly);
  m_ui->stationWidget->setReadOnly(readOnly);
}

} // metro
