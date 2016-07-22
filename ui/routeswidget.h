#ifndef METROMAP_UI_ROUTESWIDGET_H
#define METROMAP_UI_ROUTESWIDGET_H

#include <QMultiMap>
#include <QPair>
#include <QString>
#include <QWidget>

namespace Ui {
  class RoutesWidget;
} // Ui

namespace metro {

class MetroMapMainWindow;

class RoutesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit RoutesWidget(MetroMapMainWindow* ctrl, QWidget* parent = 0);
  ~RoutesWidget();

signals:
  void routeCreated(const QList<quint32>& route);

private slots:
  void slotCalcRoute();
  void slotMapChanged();

private:
  bool isMinimizeCrossovers() const;

private:
  Ui::RoutesWidget* m_ui;

  MetroMapMainWindow* m_controller;

  QMultiMap<quint32, QPair<quint32, QString> > m_stations;

};

} // metro

#endif // METROMAP_UI_ROUTESWIDGET_H
