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
  void slotSelectStation(quint32 id);
  void slotAddStation(quint32 id);
  void slotRemoveStation(quint32 id);
  void slotClear();

private slots:
  void slotMapChanged();
  void slotChangeMode();
  void slotAddNextStation();
  void slotAddCrossStation();
  void slotRemoveNeighbour();

private:
  bool setEditMode();
  bool setShowMode();
  void saveStation();

  void initLineColors();
  void selectLineColor(quint32 line);
  QColor selectedColor() const;

private:
  Ui::StationInfoWidget* m_ui;

  MetroMapMainWindow* m_controller;

  QMultiMap<quint32, QPair<quint32, QString> > m_stations;

  bool m_lockMode;
  quint32 m_currentStation;

};

} // metro

#endif // METROMAP_UI_STATIONWIDGET_H
