#include "selectstationdialog.h"
#include "ui_selectstationdialog.h"

#include <QMultiMap>
#include <QPair>
#include <QStringList>
#include <QTreeWidgetItem>

namespace {

enum column_t {
  ID = 0,
  NAME,
  MAX_COLUMN
};

bool alphabetLessThen(const QPair<quint32, QString>& lhs, const QPair<quint32, QString>& rhs)
{
  return lhs.second < rhs.second;
}

bool idLessThen(const QPair<quint32, QString>& lhs, const QPair<quint32, QString>& rhs)
{
  return lhs.first < rhs.first;
}

template <typename T> QList<T> unique(const QList<T>& repeated)
{
  QList<T> uniq;
  QListIterator<T> it(repeated);
  while (it.hasNext()) {
    const T& each = it.next();
    if (!uniq.contains(each)) {
      uniq.append(each);
    }
  }
  return uniq;
}

}

namespace metro {

SelectStationDialog::SelectStationDialog(QMultiMap<quint32, QPair<quint32, QString> >* stations, QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::SelectStationDialog()),
  m_stations(stations)
{
  m_ui->setupUi(this);

  m_ui->nameTreeWidget->setColumnCount(MAX_COLUMN);
  m_ui->nameTreeWidget->setColumnHidden(ID, true);

  connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));

  connect(m_ui->byAlphabetButton, SIGNAL(toggled(bool)), SLOT(slotChangeOrderType(bool)));
  connect(m_ui->byLinesButton, SIGNAL(toggled(bool)), SLOT(slotChangeOrderType(bool)));

  connect(m_ui->nameTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(accept()));

  m_ui->byAlphabetButton->setChecked(true);
}

SelectStationDialog::~SelectStationDialog()
{
  delete m_ui;
  m_ui = 0;
}

quint32 SelectStationDialog::selectedStation() const
{
  if (m_stations != 0) {
    if (m_ui->nameTreeWidget->selectionModel()->hasSelection()) {
      QTreeWidgetItem* item = m_ui->nameTreeWidget->selectedItems().first();
      quint32 id = 0;
      bool ok = false;
      do {
        id = item->text(ID).toUInt(&ok);
        if (ok && id > 0) {
          return id;
        }
        item = m_ui->nameTreeWidget->itemBelow(item);
      } while (item != 0 && id == 0);
    }

    if (m_ui->nameTreeWidget->topLevelItemCount() > 0) {
      return m_stations->begin().value().first;
    }
  }
  return 0;
}

void SelectStationDialog::slotChangeOrderType(bool checked)
{
  if (checked) {
    QPushButton* down = qobject_cast<QPushButton*>(sender());
    if (down != 0) {
      QPushButton* up = (down == m_ui->byAlphabetButton) ? m_ui->byLinesButton
                                                         : m_ui->byAlphabetButton;
      up->setChecked(false);
    }

    if (m_ui->byAlphabetButton->isChecked()) {
      renderStations(stationsByAlphabet());
    }
    else if (m_ui->byLinesButton->isChecked()) {
      renderStations(stationsByLines());
    }
  }
}

QList<QPair<quint32, QString> > SelectStationDialog::stationsByAlphabet() const
{
  QList<QPair<quint32, QString> > allStations;
  if (m_stations != 0) {
    QMapIterator<quint32, QPair<quint32, QString> > it(*m_stations);
    while (it.hasNext()) {
      allStations.append(it.next().value());
    }
    qSort(allStations.begin(), allStations.end(), &::alphabetLessThen);
  }
  return allStations;
}

QList<QPair<quint32, QString> > SelectStationDialog::stationsByLines() const
{
  QList<QPair<quint32, QString> > allStations;
  if (m_stations != 0) {
    const quint32 zero = 0;
    foreach (quint32 line, ::unique(m_stations->keys())) {
      QList<QPair<quint32, QString> > lineStations = m_stations->values(line);
      qSort(lineStations.begin(), lineStations.end(), &::idLessThen);
      allStations.append(qMakePair(zero, QObject::tr("Line %1").arg(line)));
      allStations.append(lineStations);
    }
  }
  return allStations;
}

void SelectStationDialog::renderStations(const QList<QPair<quint32, QString> >& stations)
{
  m_ui->nameTreeWidget->clear();

  QListIterator<QPair<quint32, QString> > it(stations);
  while (it.hasNext()) {
    const QPair<quint32, QString>& each = it.next();
    QTreeWidgetItem* item = new QTreeWidgetItem(m_ui->nameTreeWidget);
    item->setText(ID, QString::number(each.first));
    item->setText(NAME, each.second);
    item->setTextAlignment(NAME, Qt::AlignHCenter);
    if (each.first == 0) {
      QFont font = item->font(NAME);
      font.setBold(true);
      item->setFont(NAME, font);
    }
    m_ui->nameTreeWidget->addTopLevelItem(item);
  }

  m_ui->nameTreeWidget->resizeColumnToContents(NAME);

  if (m_ui->nameTreeWidget->topLevelItemCount() > 0) {
    m_ui->nameTreeWidget->topLevelItem(0)->setSelected(true);
  }
}

} // metro
