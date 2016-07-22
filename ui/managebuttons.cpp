#include "managebuttons.h"
#include "ui_managebuttons.h"

namespace metro {

ManageButtons::ManageButtons(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::ManageButtons())
{
  m_ui->setupUi(this);

  connect(m_ui->addButton, SIGNAL(clicked()), SIGNAL(addClicked()));
  connect(m_ui->moveButton, SIGNAL(clicked()), SIGNAL(moveClicked()));
}

ManageButtons::~ManageButtons()
{
  delete m_ui;
  m_ui = 0;
}

} // metro
