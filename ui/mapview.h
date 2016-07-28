#ifndef METROMAP_UI_MAPVIEW_H
#define METROMAP_UI_MAPVIEW_H

#include <QPointF>
#include <QWidget>

template <typename T> class QList;

namespace Ui {
  class MapView;
} // Ui

namespace metro {

class CrossOverItem;
class MetroMapMainWindow;
class StationItem;
class RailTrackItem;

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
  void slotShowRoute(const QList<quint32>& stations);
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

  StationItem* stationItemById(quint32 id) const;
  RailTrackItem* railTrackItemById(quint32 from, quint32 to) const;
  CrossOverItem* crossOverItemById(quint32 from, quint32 to) const;

  void clearSelection();
  void transparenceRoute(const QList<quint32>& route);

  QList<quint32> selectedStations() const;
  void selectStation(StationItem* item);
  StationItem* addStation(quint32 id, const QColor& color, const QPointF& pos = QPointF());

  void setEnableStationsMoving(bool enable);

  void showContextMenu(const QPoint& pos);

  bool eventFilter(QObject* watched, QEvent* event);

private:
  Ui::MapView* m_ui;

  MetroMapMainWindow* m_controller;

  mode_t m_mode;
  bool m_hasRoute;

  quint32 m_from;
  quint32 m_to;

};

} // metro

#endif // METROMAP_UI_MAPVIEW_H
