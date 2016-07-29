#ifndef METROMAP_UI_EDITMODEITEMS_H
#define METROMAP_UI_EDITMODEITEMS_H

#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

class QGraphicsTextItem;
class QLineF;
class QPointF;
class QRectF;
class QToolButton;

namespace metro {

class LabelItem : public QGraphicsRectItem
{
public:
  explicit LabelItem(const QPointF& topleft, QGraphicsItem* parent = 0);
  ~LabelItem();

  static QRectF labelRect(const QPointF& topleft);

  QToolButton* addButton();
  QToolButton* removeButton();

private:
  QGraphicsTextItem* m_text;

  QToolButton* m_add;
  QToolButton* m_remove;

};

class EditModeLineItem : public QGraphicsLineItem
{
public:
  explicit EditModeLineItem(const QLineF& line, QGraphicsItem* parent = 0);
  ~EditModeLineItem();

};


} // metro

#endif // METROMAP_UI_EDITMODEITEMS_H
