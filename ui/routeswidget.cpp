#include "routeswidget.h"
#include "ui_routeswidget.h"

namespace metro {

RoutesWidget::RoutesWidget(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::RoutesWidget())
{
  m_ui->setupUi(this);
}

RoutesWidget::~RoutesWidget()
{
  delete m_ui;
  m_ui = 0;
}

} // metro
