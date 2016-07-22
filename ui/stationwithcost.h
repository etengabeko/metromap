#ifndef METROMAP_UI_STATIONWITHCOST_H
#define METROMAP_UI_STATIONWITHCOST_H

#include <QWidget>

namespace Ui {
class StationWithCost;
} // Ui

namespace metro {

class StationWithCost : public QWidget
{
  Q_OBJECT

public:
  explicit StationWithCost(QWidget* parent = 0);
  ~StationWithCost();

  void setStation(quint32 id);
  void setCost(qint32 cost);

signals:
  void removeClicked();

private:
  Ui::StationWithCost* m_ui;

};

} // metro

#endif // METROMAP_UI_STATIONWITHCOST_H
