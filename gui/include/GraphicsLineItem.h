#ifndef GRAPHICSLINEITEM
#define GRAPHICSLINEITEM

#include <QGraphicsLineItem>
#include <QPair>

class GraphicsPointItem;

class GraphicsLineItem: public QGraphicsLineItem {
public:
  GraphicsLineItem(GraphicsPointItem *src, GraphicsPointItem *dest,
                   QGraphicsItem *parent = 0);
  ~GraphicsLineItem();

  GraphicsPointItem* src() { return mSrc; }
  GraphicsPointItem* dest() { return mDest; }

  void setSrc(GraphicsPointItem *src);
  void setDest(GraphicsPointItem *dest);

  void invertConnection();
  void updateEnds();
  QPair<GraphicsPointItem*, GraphicsLineItem*> splitLine();
  GraphicsLineItem* splitLine(GraphicsPointItem *point);

  void cellSizeChanged(double cellSize);

protected:
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget * widget = 0);

private:
  QColor mColor;
  GraphicsPointItem *mSrc, *mDest;

};

#endif // GRAPHICSLINEITEM
