#ifndef METROMAP_UI_FINDROUTES_H
#define METROMAP_UI_FINDROUTES_H

#include <QMultiMap>
#include <QPair>
#include <QString>
#include <QWidget>

namespace Ui {
  class FindRoutesWidget;
} // Ui

namespace metro {

class MetroMapMainWindow;

class FindRoutesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit FindRoutesWidget(MetroMapMainWindow* ctrl, QWidget* parent = 0);
  ~FindRoutesWidget();

signals:
  void routeCreated(const QList<quint32>& route);

private slots:
  void slotMapChanged();
  void slotCalcRoute();
  void slotAddMediateStation();
  void slotRemoveMediateStation();

private:
  bool isMinimizeCrossovers() const;

  void init();

private:
  Ui::FindRoutesWidget* m_ui;

  MetroMapMainWindow* m_controller;

  QMultiMap<quint32, QPair<quint32, QString> > m_stations;

};

} // metro

#endif // METROMAP_UI_FINDROUTES_H
