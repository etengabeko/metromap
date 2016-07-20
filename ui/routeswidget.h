#ifndef METROMAP_UI_ROUTESWIDGET_H
#define METROMAP_UI_ROUTESWIDGET_H

#include <QWidget>

namespace Ui {
  class RoutesWidget;
} // Ui

namespace metro {

class RoutesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit RoutesWidget(QWidget* parent = 0);
  ~RoutesWidget();

private:
  Ui::RoutesWidget* m_ui;

};

} // metro

#endif // METROMAP_UI_ROUTESWIDGET_H
