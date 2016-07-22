#include "stationwithcost.h"
#include "ui_stationwithcost.h"

#include "selectstationwidget.h"

namespace metro {

StationWithCost::StationWithCost(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::StationWithCost())
{
  m_ui->setupUi(this);

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

} // metro
