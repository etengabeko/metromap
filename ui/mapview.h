#ifndef METROMAP_UI_MAPVIEW_H
#define METROMAP_UI_MAPVIEW_H

#include <QWidget>

namespace Ui {
  class MapView;
} // Ui

namespace metro {

class MapView : public QWidget
{
  Q_OBJECT

public:
  explicit MapView(QWidget* parent = 0);
  ~MapView();

private:
  Ui::MapView* m_ui;

};

} // metro

#endif // METROMAP_UI_MAPVIEW_H
