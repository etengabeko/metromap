#ifndef METROMAP_UI_STATIONWIDGET_H
#define METROMAP_UI_STATIONWIDGET_H

#include <QWidget>

namespace Ui {
  class StationWidget;
} // Ui

namespace metro {

class StationWidget : public QWidget
{
  Q_OBJECT

public:
  explicit StationWidget(QWidget *parent = 0);
  ~StationWidget();

private:
  Ui::StationWidget* m_ui;
};

} // metro

#endif // METROMAP_UI_STATIONWIDGET_H
