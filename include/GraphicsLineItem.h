#ifndef GRAPHICSLINEITEM
#define GRAPHICSLINEITEM

#include <QGraphicsLineItem>

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
  GraphicsPointItem* splitLine();

private:
  GraphicsPointItem *mSrc, *mDest;

};

#endif // GRAPHICSLINEITEM
