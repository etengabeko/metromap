#include "metromapmainwindow.h"
#include "ui_metromapmainwindow.h"

#include "mapview.h"
#include "findroutes.h"
#include "stationinfo.h"

#include <exception/exception.h>
#include <map/map.h>
#include <settings/settings.h>
#include <station/station.h>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QStatusBar>

namespace {

QString mapFileNameFilter() { return "JSON files (*.json);;All files (*)"; }

QString mapFilePath() { return QString("%1/../../testfiles").arg(qApp->applicationDirPath()); }

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

  createMenu();
  createDockWidgets();
  createStatusBar();
  createCentralWidget();

  menuBar()->addAction(QObject::tr("Quit"), this, SLOT(close()));
  connect(this, SIGNAL(mapChanged()), SLOT(slotMapChanged()));

  connect(m_routes, SIGNAL(routeCreated(const QList<quint32>&)), m_mapview, SLOT(slotShowRoute(const QList<quint32>&)));
  connect(m_routes, SIGNAL(stationSelected(quint32)), m_mapview, SLOT(slotSelectStations(quint32)));

  connect(m_mapview, SIGNAL(fromSelected(quint32)), m_routes, SLOT(slotSelectFrom(quint32)));
  connect(m_mapview, SIGNAL(toSelected(quint32)), m_routes, SLOT(slotSelectTo(quint32)));
  connect(m_mapview, SIGNAL(stationSelected(quint32)), m_station, SLOT(slotSelectStation(quint32)));
  connect(m_mapview, SIGNAL(stationDeselected(quint32)), m_station, SLOT(slotClear()));
  connect(m_mapview, SIGNAL(stationDeselected(quint32)), m_routes, SLOT(slotDeselect(quint32)));
  connect(m_mapview, SIGNAL(stationEdited(quint32)), SLOT(slotShowStationInfo(quint32)));
  connect(m_mapview, SIGNAL(stationAdded(quint32)), SLOT(slotShowStationInfo(quint32)));
  connect(m_mapview, SIGNAL(stationAdded(quint32)), m_station, SLOT(slotAddStation(quint32)));
  connect(m_mapview, SIGNAL(stationRemoved(quint32)), m_station, SLOT(slotRemoveStation(quint32)));
  connect(m_mapview, SIGNAL(stationRemoved(quint32)), SLOT(slotRemoveStation(quint32)));

  connect(m_station, SIGNAL(editModeActivated()), m_mapview, SLOT(slotToEditMode()));
}

MetroMapMainWindow::~MetroMapMainWindow()
{
  delete m_ui;
  m_ui = 0;
}

bool MetroMapMainWindow::init()
{
  QDialog dlg(this);
  dlg.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  dlg.setLayout(new QVBoxLayout(&dlg));
  QPushButton* newBtn = new QPushButton(QObject::tr("Create map"), &dlg);
  QPushButton* openBtn = new QPushButton(QObject::tr("Open map"), &dlg);
  dlg.layout()->addWidget(openBtn);
  dlg.layout()->addWidget(newBtn);
  connect(newBtn, SIGNAL(clicked()), SLOT(slotCreateMap()));
  connect(openBtn, SIGNAL(clicked()), SLOT(slotLoadMap()));
  connect(newBtn, SIGNAL(clicked()), &dlg, SLOT(accept()));
  connect(openBtn, SIGNAL(clicked()), &dlg, SLOT(accept()));

  int res = -1;
  do {
    res = dlg.exec();
  } while (m_map.isNull());

  return res == QDialog::Accepted;
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
  QMenu* mapMenu = menuBar()->addMenu(QObject::tr("View"));

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
  QMenu* modeMenu = menuBar()->addMenu(QObject::tr("Mode"));

  m_mapview = new MapView(this, this);
  setCentralWidget(m_mapview);

  QAction* action = new QAction(QObject::tr("Show mode"), menuBar());
  action->setObjectName("mode_show");
  connect(action, SIGNAL(triggered()), m_mapview, SLOT(slotToShowMode()));
  modeMenu->addAction(action);

  action = new QAction(QObject::tr("Edit mode"), menuBar());
  action->setObjectName("mode_edit");
  connect(action, SIGNAL(triggered()), m_mapview, SLOT(slotToEditMode()));
  modeMenu->addAction(action);
}

void MetroMapMainWindow::slotCreateMap()
{
  if (isMapChanged()) {
    if (!trySaveMap()) {
      return;
    }
  }

  m_mapFileName.clear();
  settings::Loader::init(m_mapFileName);
  m_map.reset(new Map());
  m_needSaveMap = true;
  m_mapview->slotToEditMode();

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
    settings::Loader::init(m_mapFileName);
    m_map.reset(new Map());
    try {
      m_map->loadFromFile(m_mapFileName);
    }
    catch (Exception& e) {
      showErrorMessage(e.what());
    }
    m_needSaveMap = false;

    enableMenuActions(true);
    m_mapview->slotToShowMode();

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
    m_mapview->slotToShowMode();
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
      settings::Loader::init(m_mapFileName);
      slotMapChanged();
      m_needSaveMap = false;
      m_mapview->slotToShowMode();
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
    QMessageBox::StandardButton answer = QMessageBox::information(this, QObject::tr("Map is changed"),
                                                                  QObject::tr("Save changes before close?"),
                                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                                                  QMessageBox::Cancel);
    switch (answer) {
     case QMessageBox::Yes:
         slotSaveMap();
       break;
     case QMessageBox::No:
         m_needSaveMap = false;
       break;
     default:
       return;
    }
  }

  if (isMapChanged()) { // try again after saving
    QMessageBox::StandardButton answer = QMessageBox::warning(this, QObject::tr("Map not saved"),
                                                              QObject::tr("Continue without saving?"),
                                                              QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::No);
    if (answer == QMessageBox::No) {
      return;
    }
  }

  m_mapFileName.clear();
  settings::Loader::init(m_mapFileName);
  m_map.reset(new Map());
  m_needSaveMap = false;
  m_mapview->slotToShowMode();

  enableMenuActions(false);

  emit mapChanged();
}

void MetroMapMainWindow::slotShowStationInfo(quint32 id)
{
  Q_UNUSED(id);

  QDockWidget* dock = qobject_cast<QDockWidget*>(m_station->parent());
  if (dock != 0 && dock->isHidden()) {
    dock->setHidden(false);
  }
}

void MetroMapMainWindow::slotRemoveStation(quint32 id)
{
  if (!m_map.isNull() && m_map->containsStation(id)) {
    m_map->removeStation(m_map->stationById(id));
  }
  emit mapChanged();
}

void MetroMapMainWindow::closeEvent(QCloseEvent* event)
{
  if (isMapChanged()) {
    slotCloseMap();
  }

  if (isMapChanged()) { // try again after saving
    event->ignore();
    return;
  }

  QMainWindow::closeEvent(event);
}

void MetroMapMainWindow::removeStation(quint32 id)
{
  slotRemoveStation(id);
}

void MetroMapMainWindow::insertStation(const Station& station)
{
  if (!m_map.isNull()) {
    m_map->insertStation(station);
    m_needSaveMap = true;
    emit mapChanged();
  }
}

} // metro
