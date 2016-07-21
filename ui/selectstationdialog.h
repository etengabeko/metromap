#ifndef METROMAP_UI_SELECTSTATIONDIALOG_H
#define METROMAP_UI_SELECTSTATIONDIALOG_H

#include <QDialog>

class QString;
class QStringList;
template <typename K, typename V> class QMultiMap;
template <typename K, typename V> class QPair;

namespace Ui {
class SelectStationDialog;
} // Ui

namespace metro {

class SelectStationDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SelectStationDialog(QMultiMap<quint32, QPair<quint32, QString> >* stations, QWidget* parent = 0);
  ~SelectStationDialog();

  quint32 selectedStation() const;

private slots:
  void slotChangeOrderType(bool checked);

private:
  QList<QPair<quint32, QString> > stationsByAlphabet() const;
  QList<QPair<quint32, QString> > stationsByLines() const;

  void renderStations(const QList<QPair<quint32, QString> >& stations);

private:
  Ui::SelectStationDialog* m_ui;
  QMultiMap<quint32, QPair<quint32, QString> >* m_stations;

};

} // metro

#endif // METROMAP_UI_SELECTSTATIONDIALOG_H
