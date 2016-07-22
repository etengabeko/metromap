#ifndef METROMAP_UI_STATIONWIDGET_H
#define METROMAP_UI_STATIONWIDGET_H

#include <QWidget>

namespace Ui {
  class StationWidget;
} // Ui

namespace metro {

class MetroMapMainWindow;

class StationWidget : public QWidget
{
  Q_OBJECT

public:
  explicit StationWidget(MetroMapMainWindow* ctrl, QWidget *parent = 0);
  ~StationWidget();

public slots:
  void slotStationSelected(quint32 id);

private slots:
  void slotMapChanged();
  void slotChangeMode();

private:
  void clear();
  void setEditMode();
  void setShowMode();

private:
  Ui::StationWidget* m_ui;

  MetroMapMainWindow* m_controller;

  bool m_lockMode;
  quint32 m_currentStation;

};

} // metro

#endif // METROMAP_UI_STATIONWIDGET_H
