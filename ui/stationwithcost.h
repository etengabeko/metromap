#ifndef METROMAP_UI_STATIONWITHCOST_H
#define METROMAP_UI_STATIONWITHCOST_H

#include <QWidget>

template <typename K, typename V> class QMultiMap;

namespace Ui {
class StationWithCost;
} // Ui

namespace metro {

class StationWithCost : public QWidget
{
  Q_OBJECT

public:
  explicit StationWithCost(QMultiMap<quint32, QPair<quint32, QString> >* stations, QWidget* parent = 0);
  ~StationWithCost();

  quint32 station() const;
  qint32 cost() const;

  void setStation(quint32 id);
  void setCost(qint32 cost);

  void setReadOnly(bool readOnly);

signals:
  void removeClicked();

private:
  Ui::StationWithCost* m_ui;

};

} // metro

#endif // METROMAP_UI_STATIONWITHCOST_H
