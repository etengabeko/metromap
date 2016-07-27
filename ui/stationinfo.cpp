#include "stationinfo.h"
#include "ui_stationinfo.h"

#include "metromapmainwindow.h"
#include "selectstation.h"
#include "stationwithcost.h"

#include <exception/exception.h>
#include <map/map.h>
#include <station/station.h>

#include <QBoxLayout>
#include <QMessageBox>

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
  connect(m_ui->addNextButton, SIGNAL(clicked()), SLOT(slotAddNextStation()));
  connect(m_ui->addCrossButton, SIGNAL(clicked()), SLOT(slotAddCrossStation()));
}

StationInfoWidget::~StationInfoWidget()
{
  slotClear();

  delete m_ui;
  m_ui = 0;
}

void StationInfoWidget::slotSelectStation(quint32 id)
{
  slotClear();
  m_currentStation = id;

  if (m_controller->map().containsStation(id)) {
    const Station& currentStation = m_controller->map().stationById(id);
    m_ui->nameLineEdit->setText(currentStation.name());
    QString line = currentStation.line() > 0 ? QString::number(currentStation.line())
                                             : QString::null;
    m_ui->lineLineEdit->setText(line);

    foreach (quint32 id, currentStation.railTracks()) {
      bool ok = false;
      qint32 cost = currentStation.minimumCostTo(id, 0, &ok);
      if (ok) {
        StationWithCost* sc = new StationWithCost(&m_stations, this);
        connect(sc, SIGNAL(removeClicked()), SLOT(slotRemoveNeighbour()));
        sc->setReadOnly(m_lockMode);
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
        connect(sc, SIGNAL(removeClicked()), SLOT(slotRemoveNeighbour()));
        sc->setReadOnly(m_lockMode);
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

void StationInfoWidget::slotAddStation(quint32 id)
{
  slotClear();
  if (m_lockMode) {
    slotChangeMode();
  }
  m_currentStation = id;
}

void StationInfoWidget::slotRemoveStation(quint32 id)
{
  Q_UNUSED(id);

  slotClear();
  if (!m_lockMode) {
    slotChangeMode();
  }
  m_currentStation = 0;
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
  bool ok = false;
  if (m_lockMode) {
    ok = setEditMode();
    if (ok) {
      emit editModeActivated();
    }
  }
  else {
    ok = setShowMode();
    if (ok) {
      emit showModeActivated();
    }
  }

  if (ok) {
    m_lockMode = !m_lockMode;
  }
}

bool StationInfoWidget::setEditMode()
{
  m_ui->lockButton->setText(QObject::tr("Save"));
  m_ui->nameLineEdit->setReadOnly(false);
  m_ui->lineLineEdit->setReadOnly(false);
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
  return true;
}

bool StationInfoWidget::setShowMode()
{
  try {
    saveStation();
  }
  catch (Exception& e) {
    QMessageBox::warning(this, QObject::tr("Station not saved"), QString(e.what()));
    return false;
  }

  m_ui->lockButton->setText(QObject::tr("Edit"));
  m_ui->nameLineEdit->setReadOnly(true);
  m_ui->lineLineEdit->setReadOnly(true);
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
  return true;
}

void StationInfoWidget::saveStation()
{
  if (m_currentStation > 0) {
    bool ok = false;
    Station st;
    quint32 line = m_ui->lineLineEdit->text().toUInt(&ok);
    if (!ok || line == 0) {
      throw Exception(QObject::tr("Station line must be a number greater then 0"));
    }
    st.setLine(line);
    if (m_ui->nameLineEdit->text().isEmpty()) {
      throw Exception(QObject::tr("Station name must be no empty"));
    }
    st.setName(m_ui->nameLineEdit->text());

    foreach (StationWithCost* each, m_ui->nextGroupBox->findChildren<StationWithCost*>()) {
      if (each != 0 && each->station() > 0 && each->cost() > 0) {
        st.addRailTrack(each->station(), each->cost());
      }
    }
    foreach (StationWithCost* each, m_ui->crossoverGroupBox->findChildren<StationWithCost*>()) {
      if (each != 0 && each->station() > 0 && each->cost() > 0) {
        st.addCrossOver(each->station(), each->cost());
      }
    }

    if (!m_controller->map().stationsId().isEmpty()) {
      if (st.railTracks().isEmpty() && st.crossOvers().isEmpty()) {
        throw Exception(QObject::tr("Station must be have railtracks or crossovers"));
      }
    }

    if (m_controller->map().containsStation(m_currentStation)) {
      st.setId(m_currentStation);
      m_controller->removeStation(m_currentStation);
    }
    else {
      quint32 id = line*10 + 1;
      while (m_controller->map().containsStation(id)) {
        ++id;
      }
      st.setId(id);
    }
    m_controller->insertStation(st);
  }
}

void StationInfoWidget::slotAddNextStation()
{
  StationWithCost* sc = new StationWithCost(&m_stations, this);
  connect(sc, SIGNAL(removeClicked()), SLOT(slotRemoveNeighbour()));
  sc->setReadOnly(false);
  QBoxLayout* lo = qobject_cast<QBoxLayout*>(m_ui->nextGroupBox->layout());
  if (lo != 0) {
    lo->insertWidget(lo->count()-1, sc);
  }
}

void StationInfoWidget::slotAddCrossStation()
{
  StationWithCost* sc = new StationWithCost(&m_stations, this);
  connect(sc, SIGNAL(removeClicked()), SLOT(slotRemoveNeighbour()));
  sc->setReadOnly(false);
  QBoxLayout* lo = qobject_cast<QBoxLayout*>(m_ui->crossoverGroupBox->layout());
  if (lo != 0) {
    lo->insertWidget(lo->count()-1, sc);
  }
}

void StationInfoWidget::slotRemoveNeighbour()
{
  sender()->deleteLater();
}

} // metro
