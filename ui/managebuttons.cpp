#include "managebuttons.h"
#include "ui_managebuttons.h"

#include "selectstation.h"

namespace metro {

ManageButtons::ManageButtons(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::ManageButtons()),
  m_above(0),
  m_below(0)
{
  m_ui->setupUi(this);

  connect(m_ui->addButton, SIGNAL(clicked()), SIGNAL(addClicked()));
  connect(m_ui->moveButton, SIGNAL(clicked()), SIGNAL(moveClicked()));
  connect(m_ui->moveButton, SIGNAL(clicked()), SLOT(slotSwapStations()));
}

ManageButtons::~ManageButtons()
{
  delete m_ui;
  m_ui = 0;
}

void ManageButtons::setManagedWidgets(SelectStationWidget* above, SelectStationWidget* below)
{
  m_above = above;
  m_below = below;
}

SelectStationWidget* ManageButtons::above()
{
  return m_above;
}

SelectStationWidget* ManageButtons::below()
{
  return m_below;
}

void ManageButtons::slotSwapStations()
{
  if (m_above != 0 && m_below != 0) {
    quint32 above = m_above->selectedStation();
    quint32 below = m_below->selectedStation();
    m_above->slotSelectStation(below);
    m_below->slotSelectStation(above);
  }
}

} // metro
