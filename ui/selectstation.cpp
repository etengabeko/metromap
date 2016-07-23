#include "selectstation.h"
#include "ui_selectstation.h"

#include "selectstationdialog.h"

#include <map/map.h>
#include <station/station.h>

#include <QCompleter>
#include <QMultiMap>
#include <QPair>
#include <QString>

namespace metro {

SelectStationWidget::SelectStationWidget(QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::SelectStationWidget()),
  m_stations(0),
  m_currentStation(0)
{
  m_ui->setupUi(this);

  connect(m_ui->advancedButton, SIGNAL(clicked()), SLOT(slotOpenAdvancedDialog()));
  connect(m_ui->nameLineEdit, SIGNAL(editingFinished()), SLOT(slotNameChanged()));
  connect(m_ui->nameLineEdit, SIGNAL(returnPressed()), SLOT(slotNameChanged()));
}

SelectStationWidget::~SelectStationWidget()
{
  delete m_ui;
  m_ui = 0;
}

quint32 SelectStationWidget::selectedStation() const
{
  return m_currentStation;
}

void SelectStationWidget::setPlaceholderText(const QString& str)
{
  m_ui->nameLineEdit->setPlaceholderText(str);
}

void SelectStationWidget::setStations(QMultiMap<quint32, QPair<quint32, QString> >* stations)
{
  m_ui->nameLineEdit->clear();

  m_stations = stations;

  QStringList complist;
  if (m_stations != 0) {
    QMapIterator<quint32, QPair<quint32, QString> > it(*m_stations);
    while (it.hasNext()) {
      complist.append(it.next().value().second);
    }
  }

  m_completer.reset(new QCompleter(complist));
  m_completer->setCaseSensitivity(Qt::CaseInsensitive);
  m_ui->nameLineEdit->setCompleter(m_completer.data());
}

QMultiMap<quint32, QPair<quint32, QString> > SelectStationWidget::getStationsByLines(const Map& map)
{
  QMultiMap<quint32, QPair<quint32, QString> > result;
  foreach (quint32 id, map.stationsId()) {
    const Station& each = map.stationById(id);
    result.insert(each.line(), qMakePair(each.id(), each.name()));
  }
  return result;
}

void SelectStationWidget::slotNameChanged()
{
  QString name = m_ui->nameLineEdit->text();
  if (m_stations != 0 && !name.isEmpty()) {
    QMapIterator<quint32, QPair<quint32, QString> > it(*m_stations);
    while (it.hasNext()) {
      const QPair<quint32, QString>& each = it.next().value();
      if (each.second == name) {
        m_currentStation = each.first;
        emit stationSelected(each.first);
      }
    }
  }
}

void SelectStationWidget::slotSelectStation(quint32 id)
{
  if (m_stations != 0) {
    QMapIterator<quint32, QPair<quint32, QString> > it(*m_stations);
    while (it.hasNext()) {
      const QPair<quint32, QString>& each = it.next().value();
      if (each.first == id) {
        m_currentStation = id;
        m_ui->nameLineEdit->setText(each.second);
        return;
      }
    }
  }
  m_currentStation = 0;
  m_ui->nameLineEdit->clear();
}

void SelectStationWidget::slotOpenAdvancedDialog()
{
  SelectStationDialog dlg(m_stations, this);
  if (dlg.exec() == QDialog::Accepted) {
    quint32 id = dlg.selectedStation();
    slotSelectStation(id);
    emit stationSelected(id);
  }
}

} // metro
