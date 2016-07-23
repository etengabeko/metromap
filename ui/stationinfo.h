#ifndef METROMAP_UI_STATIONINFO_H
#define METROMAP_UI_STATIONINFO_H

#include <QWidget>

namespace Ui {
  class StationInfoWidget;
} // Ui

namespace metro {

class MetroMapMainWindow;

class StationInfoWidget : public QWidget
{
  Q_OBJECT

public:
  explicit StationInfoWidget(MetroMapMainWindow* ctrl, QWidget *parent = 0);
  ~StationInfoWidget();

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
  Ui::StationInfoWidget* m_ui;

  MetroMapMainWindow* m_controller;

  bool m_lockMode;
  quint32 m_currentStation;

};

} // metro

#endif // METROMAP_UI_STATIONWIDGET_H
