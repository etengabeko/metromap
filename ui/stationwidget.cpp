#include "stationwidget.h"
#include "ui_stationwidget.h"

namespace metro {

StationWidget::StationWidget(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::StationWidget())
{
  m_ui->setupUi(this);
}

StationWidget::~StationWidget()
{
  delete m_ui;
  m_ui = 0;
}

} // metro
