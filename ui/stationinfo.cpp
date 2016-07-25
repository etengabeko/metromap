#include "stationinfo.h"
#include "ui_stationinfo.h"

#include "metromapmainwindow.h"
#include "selectstation.h"
#include "stationwithcost.h"

#include <map/map.h>
#include <station/station.h>

#include <QBoxLayout>

namespace metro {

StationInfoWidget::StationInfoWidget(MetroMapMainWindow* ctrl, QWidget* parent) :
  QWidget(parent),
  m_ui(new Ui::StationInfoWidget()),
  m_controller(ctrl),
  m_lockMode(true),
  m_currentStation(0)
{
  Q_CHECK_PTR(m_controller);

  m_ui->setupUi(this);

  setShowMode();

  connect(m_controller, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
  connect(m_ui->lockButton, SIGNAL(clicked()), SLOT(slotChangeMode()));
}

StationInfoWidget::~StationInfoWidget()
{
  slotClear();

  delete m_ui;
  m_ui = 0;
}

void StationInfoWidget::slotStationSelected(quint32 id)
{
  slotClear();
  setShowMode();

  if (m_controller->map().containsStation(id)) {
    m_currentStation = id;
    const Station& currentStation = m_controller->map().stationById(id);
    m_ui->nameLineEdit->setText(currentStation.name());
    m_ui->lineLineEdit->setText(QString::number(currentStation.line()));

    foreach (quint32 id, currentStation.railTracks()) {
      bool ok = false;
      qint32 cost = currentStation.minimumCostTo(id, 0, &ok);
      if (ok) {
        StationWithCost* sc = new StationWithCost(&m_stations, this);
        sc->setReadOnly(true);
        sc->setStation(id);
        sc->setCost(cost);
        QBoxLayout* lo = qobject_cast<QBoxLayout*>(m_ui->nextGroupBox->layout());
        if (lo != 0) {
          lo->insertWidget(lo->count()-1, sc);
        }
      }
    }

    foreach (quint32 id, currentStation.crossOvers()) {
      bool ok = false;
      qint32 cost = currentStation.minimumCostTo(id, 0, &ok);
      if (ok) {
        StationWithCost* sc = new StationWithCost(&m_stations, this);
        sc->setReadOnly(true);
        sc->setStation(id);
        sc->setCost(cost);
        QBoxLayout* lo = qobject_cast<QBoxLayout*>(m_ui->crossoverGroupBox->layout());
        if (lo != 0) {
          lo->insertWidget(lo->count()-1, sc);
        }
      }
    }
  }
}

void StationInfoWidget::slotMapChanged()
{
  if (!m_ui->lockButton->isEnabled()) {
    m_ui->lockButton->setEnabled(true);
  }
  slotClear();

  m_stations = SelectStationWidget::getStationsByLines(m_controller->map());
}

void StationInfoWidget::slotClear()
{
  m_currentStation = 0;
  m_ui->nameLineEdit->clear();
  m_ui->lineLineEdit->clear();
  m_ui->lineComboBox->clear();
  foreach (QGroupBox* box, findChildren<QGroupBox*>()) {
    if (box != 0) {
      foreach (StationWithCost* st, box->findChildren<StationWithCost*>()) {
        delete st;
      }
    }
  }
}

void StationInfoWidget::slotChangeMode()
{
  if (m_lockMode) {
    setEditMode();
  }
  else {
    setShowMode();
  }
  m_lockMode = !m_lockMode;
}

void StationInfoWidget::setEditMode()
{
  m_ui->lockButton->setText(QObject::tr("Save"));
  m_ui->nameLineEdit->setReadOnly(false);
  m_ui->lineStackedWidget->setCurrentWidget(m_ui->editPage);
  foreach (QGroupBox* box, findChildren<QGroupBox*>()) {
    if (box != 0) {
      foreach (QToolButton* btn, box->findChildren<QToolButton*>()) {
        if (btn != 0) {
          btn->setEnabled(true);
        }
      }
      foreach (StationWithCost* sc, box->findChildren<StationWithCost*>()) {
        if (sc != 0) {
          sc->setReadOnly(false);
        }
      }
    }
  }

  if (m_ui->lineComboBox->count() == 0) {
    QStringList lines;
    foreach (quint32 each, m_controller->map().linesId()) {
      lines.append(QString::number(each));
    }
    qSort(lines);
    m_ui->lineComboBox->addItems(lines);
  }

  if (   m_currentStation > 0
      && m_controller->map().containsStation(m_currentStation)) {
    int index = m_ui->lineComboBox->findText(QString::number(m_controller->map().stationById(m_currentStation).line()));
    if (index != -1) {
      m_ui->lineComboBox->setCurrentIndex(index);
    }
    else {
      m_ui->lineComboBox->setCurrentIndex(0);
    }
  }
  emit editModeActivated();
}

void StationInfoWidget::setShowMode()
{
  m_ui->lockButton->setText(QObject::tr("Edit"));
  m_ui->nameLineEdit->setReadOnly(true);
  m_ui->lineStackedWidget->setCurrentWidget(m_ui->showPage);
  foreach (QGroupBox* box, findChildren<QGroupBox*>()) {
    if (box != 0) {
      foreach (QToolButton* btn, box->findChildren<QToolButton*>()) {
        if (btn != 0) {
          btn->setEnabled(false);
        }
      }
    }
    foreach (StationWithCost* sc, box->findChildren<StationWithCost*>()) {
      if (sc != 0) {
        sc->setReadOnly(true);
      }
    }

  }
  emit showModeActivated();
}

} // metro
