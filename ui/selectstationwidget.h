#ifndef METROMAP_UI_SELECTSTATIONWIDGET_H
#define METROMAP_UI_SELECTSTATIONWIDGET_H

#include <QScopedPointer>
#include <QWidget>

class QCompleter;
class QString;
template <typename K, typename V> class QMultiMap;
template <typename K, typename V> class QPair;

namespace Ui {
class SelectStationWidget;
} // Ui

namespace metro {

class Map;

class SelectStationWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SelectStationWidget(QWidget* parent = 0);
  ~SelectStationWidget();

  void setStations(QMultiMap<quint32, QPair<quint32, QString> >* stations);

  static QMultiMap<quint32, QPair<quint32, QString> > getStationsByLines(const Map& map);

signals:
  void stationSelected(quint32 id);

public slots:
  void slotSelectStation(quint32 id);

private slots:
  void slotOpenAdvancedDialog();
  void slotNameChanged();

private:
  Ui::SelectStationWidget* m_ui;
  QScopedPointer<QCompleter> completer_;
  QMultiMap<quint32, QPair<quint32, QString> >* m_stations;

};

} // metro

#endif // METROMAP_UI_SELECTSTATIONWIDGET_H
