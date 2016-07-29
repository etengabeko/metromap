#include "editmodeitems.h"

#include <QBrush>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QLineF>
#include <QObject>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QToolButton>

namespace metro {

LabelItem::LabelItem(const QPointF& topleft, QGraphicsItem* parent) :
  QGraphicsRectItem(labelRect(topleft), parent),
  m_text(new QGraphicsTextItem(QObject::tr("Edit Mode"), this)),
  m_add(new QToolButton()),
  m_remove(new QToolButton())
{
  QBrush b;
  b.setStyle(Qt::SolidPattern);
  QColor c(Qt::yellow);
  c.setAlpha(127);
  b.setColor(c);
  setBrush(b);

  QPointF p(topleft.x()+10.0, topleft.y()+5.0);
  m_text->setPos(p);

  m_add->setText("+");
  m_remove->setText("−");

  QSize sz(32,32);
  m_add->setMinimumSize(sz);
  m_remove->setMinimumSize(sz);

// FIXME
//  QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
//  proxy->setWidget(m_add);
//  p = QPointF(topleft.x()+10.0, topleft.y()+50.0);
//  proxy->setPos(p);

//  proxy = new QGraphicsProxyWidget(this);
//  proxy->setWidget(m_remove);
//  p = QPointF(topleft.x()+50.0, topleft.y()+50.0);
//  proxy->setPos(p);
}

LabelItem::~LabelItem()
{
  delete m_add;    m_add = 0;
  delete m_remove; m_remove = 0;
}

QRectF LabelItem::labelRect(const QPointF& topleft)
{
  const qreal w = 90.0,
              h = 40.0;
  return QRectF(topleft, QSizeF(w,h));
}

QToolButton* LabelItem::addButton()
{
  return m_add;
}

QToolButton* LabelItem::removeButton()
{
  return m_remove;
}

EditModeLineItem::EditModeLineItem(const QLineF& line, QGraphicsItem* parent) :
  QGraphicsLineItem(line, parent)
{
  QPen p;
  p.setStyle(Qt::DashLine);
  p.setColor(Qt::lightGray);
  setPen(p);
}

EditModeLineItem::~EditModeLineItem()
{
}

} // metro
