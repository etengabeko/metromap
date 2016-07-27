#ifndef METROMAP_UI_METROMAPMAINWINDOW_H
#define METROMAP_UI_METROMAPMAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QString>

class QCloseEvent;

namespace Ui {
  class MetroMapWidget;
} // Ui

namespace metro {

class Map;
class MapView;
class FindRoutesWidget;
class Station;
class StationInfoWidget;

class MetroMapMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MetroMapMainWindow(QWidget* parent = 0);
  ~MetroMapMainWindow();

  bool init();

  bool isMapChanged() const;

  const Map& map()const;

  void removeStation(quint32 id);
  void insertStation(const Station& station);
  void showErrorMessage(const QString& error);

signals:
  void mapChanged();

private slots:
  void slotCreateMap();
  void slotLoadMap();
  void slotSaveMap();
  void slotSaveAsMap();
  void slotCloseMap();
  void slotMapChanged();
  void slotShowStationInfo(quint32 id);
  void slotRemoveStation(quint32 id);

private:
  void closeEvent(QCloseEvent* event);

  void createMenu();
  void createDockWidgets();
  void createStatusBar();
  void createCentralWidget();

  bool trySaveMap();
  bool trySaveMap(const QString& fileName);


  void enableMenuActions(bool isMapOpen);

private:
  Ui::MetroMapWidget* m_ui;

  MapView* m_mapview;
  FindRoutesWidget* m_routes;
  StationInfoWidget* m_station;

  QScopedPointer<Map> m_map;
  QString m_mapFileName;
  bool m_needSaveMap;

};

} // metro

#endif // METROMAP_UI_METROMAPMAINWINDOW_H
