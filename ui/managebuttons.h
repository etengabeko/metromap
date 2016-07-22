#ifndef METROMAP_UI_MANAGEBUTTONS_H
#define METROMAP_UI_MANAGEBUTTONS_H

#include <QWidget>

namespace Ui {
class ManageButtons;
} // Ui

namespace metro {

class ManageButtons : public QWidget
{
  Q_OBJECT

public:
  explicit ManageButtons(QWidget* parent = 0);
  ~ManageButtons();

signals:
  void moveClicked();
  void addClicked();

private:
  Ui::ManageButtons* m_ui;

};

} // metro

#endif // METROMAP_UI_MANAGEBUTTONS_H
