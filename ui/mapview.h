#ifndef METROMAP_UI_MAPVIEW_H
#define METROMAP_UI_MAPVIEW_H

#include <QPointF>
#include <QWidget>

template <typename T> class QList;

namespace Ui {
  class MapView;
} // Ui

namespace metro {

class MetroMapMainWindow;
class StationItem;

class MapView : public QWidget
{
  Q_OBJECT

  enum mode_t {
    SHOW,
    EDIT
  };

public:
  explicit MapView(MetroMapMainWindow* ctrl, QWidget* parent = 0);
  ~MapView();

signals:
  void stationSelected(quint32 id);
  void stationDeselected(quint32 id);
  void fromSelected(quint32 id);
  void toSelected(quint32 id);
  void stationEdited(quint32 id);
  void stationAdded(quint32 id);
  void stationRemoved(quint32 id);

public slots:
  void slotSelectStations(quint32 id);
  void slotSelectStations(const QList<quint32>& stations);
  void slotShowRouteInfo(const QList<quint32>& stations);
  void slotToShowMode();
  void slotToEditMode();

private slots:
  void slotMapChanged();
  void slotDeselectStation(quint32 id);

private:
  void initScene();
  void renderStations();
  void renderRailTracks();
  void renderCrossOvers();

  StationItem* itemById(quint32 id) const;

  void clearSelection();
  QList<quint32> selectedStations() const;
  void selectStation(StationItem* item, const QPoint& pos);
  StationItem* addStation(quint32 id, const QColor& color, const QPointF& pos = QPointF());

  void setEnableStationsMoving(bool enable);

  void showContextMenu(const QPoint& pos);

  bool eventFilter(QObject* watched, QEvent* event);

private:
  Ui::MapView* m_ui;

  MetroMapMainWindow* m_controller;

  mode_t m_mode;

  quint32 m_from;
  quint32 m_to;

};

} // metro

#endif // METROMAP_UI_MAPVIEW_H
