#ifndef METROMAP_UI_MANAGEBUTTONS_H
#define METROMAP_UI_MANAGEBUTTONS_H

#include <QWidget>

namespace Ui {
class ManageButtons;
} // Ui

namespace metro {

class SelectStationWidget;

class ManageButtons : public QWidget
{
  Q_OBJECT

public:
  explicit ManageButtons(QWidget* parent = 0);
  ~ManageButtons();

  void setManagedWidgets(SelectStationWidget* above, SelectStationWidget* below);
  SelectStationWidget* above();
  SelectStationWidget* below();

signals:
  void moveClicked();
  void addClicked();
  void removeClicked();

private slots:
  void slotSwapStations();

private:
  Ui::ManageButtons* m_ui;

  SelectStationWidget* m_above;
  SelectStationWidget* m_below;

};

} // metro

#endif // METROMAP_UI_MANAGEBUTTONS_H
