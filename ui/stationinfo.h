#ifndef METROMAP_UI_STATIONINFO_H
#define METROMAP_UI_STATIONINFO_H

#include <QMultiMap>
#include <QPair>
#include <QString>
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

signals:
  void editModeActivated();
  void showModeActivated();

public slots:
  void slotStationSelected(quint32 id);
  void slotClear();

private slots:
  void slotMapChanged();
  void slotChangeMode();
  void slotAddNextStation();
  void slotAddCrossStation();
  void slotRemoveNextStation();
  void slotRemoveCrossStation();

private:
  void setEditMode();
  void setShowMode();
  void saveStation();

private:
  Ui::StationInfoWidget* m_ui;

  MetroMapMainWindow* m_controller;

  QMultiMap<quint32, QPair<quint32, QString> > m_stations;

  bool m_lockMode;
  quint32 m_currentStation;

};

} // metro

#endif // METROMAP_UI_STATIONWIDGET_H
