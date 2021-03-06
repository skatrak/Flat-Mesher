#ifndef GRAPHICSPOINTITEM
#define GRAPHICSPOINTITEM

#include <QGraphicsEllipseItem>
#include <QObject>
#include <QPair>

#include <FlatMesher/Point2.h>

class GraphicsLineItem;

enum class HighlightMode {
  None,
  First,
  Last
};

class GraphicsPointItem: public QObject, public QGraphicsEllipseItem {
  Q_OBJECT

public:
   GraphicsPointItem(const flat::Point2& pos, QGraphicsItem *parent = 0);
  ~GraphicsPointItem();

  flat::Point2 flatPoint() const { return mPoint; }
  GraphicsLineItem* inputLine() { return mInputLine; }
  GraphicsLineItem* outputLine() { return mOutputLine; }
  bool isDetached() const;
  GraphicsPointItem* nextPoint();
  GraphicsPointItem* prevPoint();

  void setFlatPoint(const flat::Point2& point);
  void setInputLine(GraphicsLineItem *line);
  void setOutputLine(GraphicsLineItem *line);

  void setHighlight(HighlightMode highlight);
  void invertConnection();
  QPair<GraphicsLineItem*, GraphicsLineItem*> detach();

  void cellSizeChanged(double cellSize);

signals:
  void itemMoved(GraphicsPointItem *item, const flat::Point2& oldPos);

protected:
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget);

private:
  QColor mEdgesColor;

  flat::Point2 mPoint, mLastPoint;
  QPointF mInitialPos;
  double mCellSize;
  GraphicsLineItem *mInputLine, *mOutputLine;

};

#endif // GRAPHICSPOINTITEM
