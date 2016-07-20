#include "mapview.h"
#include "ui_mapview.h"

namespace metro {

MapView::MapView(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::MapView())
{
  m_ui->setupUi(this);
}

MapView::~MapView()
{
  delete m_ui;
  m_ui = 0;
}

} // metro
