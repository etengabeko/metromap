#include "metromapmainwindow.h"
#include "ui_metromapmainwindow.h"

#include "mapview.h"
#include "routeswidget.h"
#include "stationwidget.h"

#include <exception/exception.h>
#include <map/map.h>
#include <station/station.h>

#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

namespace {

QString mapFileNameFilter() { return "JSON files (*.json);;All files (*)"; }

QString mapFilePath() { return qApp->applicationDirPath(); }

}

namespace metro {

MetroMapMainWindow::MetroMapMainWindow(QWidget* parent) :
  QMainWindow(parent),
  m_ui(new Ui::MetroMapWidget()),
  m_mapview(0),
  m_routes(0),
  m_station(0),
  m_needSaveMap(false)
{
  m_ui->setupUi(this);
}

MetroMapMainWindow::~MetroMapMainWindow()
{
  delete m_ui;
  m_ui = 0;
}

void MetroMapMainWindow::init()
{
  createMenu();
  createDockWidgets();
  createCentralWidget();

  //TODO init connections
}

bool MetroMapMainWindow::isMapChanged() const
{
  return m_needSaveMap;
}

void MetroMapMainWindow::createMenu()
{
  QMenu* mapmenu = menuBar()->addMenu(QObject::tr("Map"));
  mapmenu->addAction(QObject::tr("New"), this, SLOT(slotCreateMap()));
  mapmenu->addAction(QObject::tr("Open"), this, SLOT(slotLoadMap()));
  mapmenu->addAction(QObject::tr("Save"), this, SLOT(slotSaveMap()));
  mapmenu->addAction(QObject::tr("Save as..."), this, SLOT(slotSaveAsMap()));
  mapmenu->addAction(QObject::tr("Close"), this, SLOT(slotCloseMap()));

  menuBar()->addAction(QObject::tr("Quit"), this, SLOT(close()));
}

void MetroMapMainWindow::createDockWidgets()
{
  QDockWidget* dock = new QDockWidget(QObject::tr("Station Info"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  m_station = new StationWidget(dock);
  dock->setWidget(m_station);
  addDockWidget(Qt::RightDockWidgetArea, dock);

  dock = new QDockWidget(QObject::tr("Routes"), this);
  dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  m_routes = new RoutesWidget(dock);
  dock->setWidget(m_routes);
  addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void MetroMapMainWindow::createCentralWidget()
{
  m_mapview = new MapView(this);
  setCentralWidget(m_mapview);
}

void MetroMapMainWindow::slotCreateMap()
{
  if (isMapChanged()) {
    if (!trySaveMap()) {
      return;
    }
  }

  m_mapFileName.clear();
  m_map.reset(new Map());
  m_needSaveMap = true;

  emit mapChanged();
}

void MetroMapMainWindow::slotLoadMap()
{
  QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("Select file..."),
                                                  ::mapFilePath(),
                                                  ::mapFileNameFilter());
  if (!fileName.isEmpty()) {
    if (fileName == m_mapFileName) {
      QMessageBox::information(this, QObject::tr("Attention"),
                               QObject::tr("Map %1 already opened").arg(fileName),
                               QMessageBox::Ok);
      return;
    }
    if (isMapChanged()) {
      if (!trySaveMap()) {
        return;
      }
    }

    m_mapFileName = fileName;
    m_map.reset(new Map());
    try {
      m_map->loadFromFile(m_mapFileName);
    }
    catch (Exception& e) {
      showErrorMessage(e.what());
    }
    m_needSaveMap = false;

    emit mapChanged();
  }
}

void MetroMapMainWindow::slotSaveMap()
{
  if (m_mapFileName.isEmpty()) {
    slotSaveAsMap();
    return;
  }

  if (trySaveMap()) {
    m_needSaveMap = false;
  }
}

void MetroMapMainWindow::slotSaveAsMap()
{
  QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("Save as..."),
                                                  ::mapFilePath(),
                                                  ::mapFileNameFilter());
  if (!fileName.isEmpty()) {
    if (trySaveMap(fileName)) {
      m_mapFileName = fileName;
      m_needSaveMap = false;
    }
  }
}

bool MetroMapMainWindow::trySaveMap()
{
  return trySaveMap(m_mapFileName);
}

bool MetroMapMainWindow::trySaveMap(const QString& fileName)
{
  try {
    if (!m_map.isNull()) {
      m_map->saveToFile(fileName);
    }
  }
  catch (Exception& e) {
    showErrorMessage(e.what());
    return false;
  }
  return true;
}

void MetroMapMainWindow::showErrorMessage(const QString& error)
{
  QMessageBox::warning(this, QObject::tr("Error"),
                       error,QMessageBox::Ok);
}

void MetroMapMainWindow::slotCloseMap()
{
  if (isMapChanged()) {
    if (!trySaveMap()) {
      QMessageBox::StandardButton answer = QMessageBox::warning(this, QObject::tr("Map not saved"),
                                                        QObject::tr("Continue without saving?"),
                                                        QMessageBox::Yes | QMessageBox::No,
                                                        QMessageBox::No);
      if (answer == QMessageBox::No) {
        return;
      }
    }
  }

  m_mapFileName.clear();
  m_map.reset();
  m_needSaveMap = false;

  emit mapChanged();
}

void MetroMapMainWindow::closeEvent(QCloseEvent* event)
{
  slotCloseMap();

  if (m_needSaveMap) {
    event->ignore();
    return;
  }

  QMessageBox::StandardButton answer = QMessageBox::information(this, QObject::tr("Confirm quit"),
                                                                QObject::tr("You really want to quit?"),
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
  if (answer == QMessageBox::No) {
    event->ignore();
    return;
  }

  QMainWindow::closeEvent(event);
}

} // metro
