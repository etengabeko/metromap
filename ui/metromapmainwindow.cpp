#include "metromapmainwindow.h"
#include "ui_metromapmainwindow.h"

#include "mapview.h"
#include "findroutes.h"
#include "stationinfo.h"

#include <exception/exception.h>
#include <map/map.h>
#include <station/station.h>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegExp>
#include <QStatusBar>

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
  createStatusBar();
  createCentralWidget();

  menuBar()->addAction(QObject::tr("Quit"), this, SLOT(close()));
  connect(this, SIGNAL(mapChanged()), SLOT(slotMapChanged()));
  //TODO init connections
  connect(m_routes, SIGNAL(routeCreated(const QList<quint32>&)), m_mapview, SLOT(slotSelectStations(const QList<quint32>&)));
}

bool MetroMapMainWindow::isMapChanged() const
{
  return m_needSaveMap;
}

const Map& MetroMapMainWindow::map() const
{
  return *m_map;
}

void MetroMapMainWindow::createMenu()
{
  QMenu* fileMenu = menuBar()->addMenu(QObject::tr("File"));

  QAction* action = new QAction(QObject::tr("New"), menuBar());
  action->setObjectName("map_new");
  connect(action, SIGNAL(triggered()), SLOT(slotCreateMap()));
  fileMenu->addAction(action);

  action = new QAction(QObject::tr("Open"), menuBar());
  action->setObjectName("map_open");
  connect(action, SIGNAL(triggered()), SLOT(slotLoadMap()));
  fileMenu->addAction(action);

  action = new QAction(QObject::tr("Save"), menuBar());
  action->setObjectName("map_save");
  action->setEnabled(false);
  connect(action, SIGNAL(triggered()), SLOT(slotSaveMap()));
  fileMenu->addAction(action);

  action = new QAction(QObject::tr("Save As..."), menuBar());
  action->setObjectName("map_save_as");
  action->setEnabled(false);
  connect(action, SIGNAL(triggered()), SLOT(slotSaveAsMap()));
  fileMenu->addAction(action);

  action = new QAction(QObject::tr("Close"), menuBar());
  action->setObjectName("map_close");
  action->setEnabled(false);
  connect(action, SIGNAL(triggered()), SLOT(slotCloseMap()));
  fileMenu->addAction(action);
}

void MetroMapMainWindow::createDockWidgets()
{
  QMenu* mapMenu = menuBar()->addMenu(QObject::tr("Map"));

  QDockWidget* dock = new QDockWidget(QObject::tr("Station Info"), this);
  dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  m_station = new StationInfoWidget(this, dock);
  dock->setWidget(m_station);
  mapMenu->addAction(dock->toggleViewAction());
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dock->setHidden(true);

  dock = new QDockWidget(QObject::tr("Find Routes"), this);
  dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  m_routes = new FindRoutesWidget(this, dock);
  dock->setWidget(m_routes);
  mapMenu->addAction(dock->toggleViewAction());
  addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MetroMapMainWindow::createStatusBar()
{
  QStatusBar* stbar = statusBar();
  QLabel* lb = new QLabel(QObject::tr("Map file:"), stbar);
  lb->setObjectName("map_name");
  stbar->addWidget(lb);
}

void MetroMapMainWindow::createCentralWidget()
{
  m_mapview = new MapView(this, this);
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

  enableMenuActions(true);

  emit mapChanged();
}

void MetroMapMainWindow::enableMenuActions(bool isMapOpen)
{
  foreach (QAction* each, menuBar()->findChildren<QAction*>(QRegExp("map_.*"))) {
    if (each != 0) {
           if (each->objectName() == "map_new")     { each->setEnabled(!isMapOpen); }
      else if (each->objectName() == "map_open")    { each->setEnabled(!isMapOpen); }
      else if (each->objectName() == "map_save")    { each->setEnabled(isMapOpen) ; }
      else if (each->objectName() == "map_save_as") { each->setEnabled(isMapOpen) ; }
      else if (each->objectName() == "map_close")   { each->setEnabled(isMapOpen) ; }
    }
  }
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

    enableMenuActions(true);

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
      slotMapChanged();
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

void MetroMapMainWindow::slotMapChanged()
{
  foreach (QLabel* each, statusBar()->findChildren<QLabel*>()) {
    if (each != 0) {
      if (each->objectName() == "map_name") { each->setText(QString("%1: %2").arg(QObject::tr("Map file")).arg(m_mapFileName)); }
    }
  }
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
  m_map.reset(new Map());
  m_needSaveMap = false;

  enableMenuActions(false);

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
