#ifndef METROMAP_UI_MAPVIEW_H
#define METROMAP_UI_MAPVIEW_H

#include <QWidget>

template <typename T> class QList;

namespace Ui {
  class MapView;
} // Ui

namespace metro {

class MetroMapMainWindow;

class MapView : public QWidget
{
  Q_OBJECT

public:
  explicit MapView(MetroMapMainWindow* ctrl, QWidget* parent = 0);
  ~MapView();

public slots:
  void slotSelectStations(quint32 id);
  void slotSelectStations(const QList<quint32>& stations);

private slots:
  void slotMapChanged();

private:
  void initScene();

private:
  Ui::MapView* m_ui;

  MetroMapMainWindow* m_controller;

};

} // metro

#endif // METROMAP_UI_MAPVIEW_H
