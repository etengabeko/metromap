#ifndef METROMAP_UI_RAILTRACKITEM_H
#define METROMAP_UI_RAILTRACKITEM_H

#include <QGraphicsLineItem>

class QColor;
class QLineF;
class QPointF;

namespace metro {

class StationItem;

class RailTrackItem : public QGraphicsLineItem
{
public:
  explicit RailTrackItem(const StationItem& from, const StationItem& to, const QColor& color, QGraphicsItem* parent = 0);
  ~RailTrackItem();

  quint32 stationFrom() const;
  quint32 stationTo() const;

  void setCoordFrom(const QPointF& pos);
  void setCoordTo(const QPointF& pos);

private:
  QLineF makeLine(const QPointF& from, const QPointF& to) const;

private:
  quint32 m_from;
  quint32 m_to;

};

} // metro

#endif // METROMAP_UI_RAILTRACKITEM_H
