#include "GraphicsLineItem.h"

#include "Configuration.h"
#include "GraphicsPointItem.h"
#include "MeshEditor.h"

#include <FlatMesher/Line2.h>

#include <QPen>

GraphicsLineItem::GraphicsLineItem(GraphicsPointItem *src, GraphicsPointItem *dest,
                                   QGraphicsItem *parent):
    QGraphicsLineItem(parent), mSrc(nullptr), mDest(nullptr) {
  setSrc(src);
  setDest(dest);

  setFlag(QGraphicsItem::ItemIsSelectable);
  setAcceptHoverEvents(true);
  setZValue(-1.0);

  cellSizeChanged(config::DEFAULT_TRIANGLE_SZ);
}

GraphicsLineItem::~GraphicsLineItem() {
  if (mSrc)
    mSrc->setOutputLine(nullptr);

  if (mDest)
    mDest->setInputLine(nullptr);
}

void GraphicsLineItem::setSrc(GraphicsPointItem *src) {
  mSrc = src;
  updateEnds();
}

void GraphicsLineItem::setDest(GraphicsPointItem *dest) {
  mDest = dest;
  updateEnds();
}

void GraphicsLineItem::invertConnection() {
  GraphicsPointItem *tmp = mSrc;
  mSrc = mDest;
  mDest = tmp;
}

void GraphicsLineItem::updateEnds() {
  if (mSrc && mDest) {
    flat::Line2 line(mSrc->flatPoint(), mDest->flatPoint());
    setLine(MeshEditor::mapFromFlat(line));
  }
  else
    setLine(QLineF());
}

QPair<GraphicsPointItem*, GraphicsLineItem*> GraphicsLineItem::splitLine() {
  if (mSrc && mDest) {
    flat::Point2 src = mSrc->flatPoint();
    flat::Point2 dest = mDest->flatPoint();
    flat::Point2 mid((src.getX() + dest.getX()) / 2.0,
                     (src.getY() + dest.getY()) / 2.0);

    GraphicsPointItem *middlePoint = new GraphicsPointItem(mid, parentItem());
    GraphicsLineItem *nextLine = splitLine(middlePoint);

    return {middlePoint, nextLine};
  }

  return {nullptr, nullptr};
}

GraphicsLineItem* GraphicsLineItem::splitLine(GraphicsPointItem *point) {
  GraphicsLineItem *nextLine = nullptr;

  if (mSrc && mDest && point) {
    nextLine = new GraphicsLineItem(point, mDest, parentItem());

    point->setOutputLine(nextLine);
    point->setInputLine(this);
    mDest->setInputLine(nextLine);

    setDest(point);
  }

  return nextLine;
}

void GraphicsLineItem::cellSizeChanged(double cellSize) {
  QPen pen(Qt::black, cellSize / 8);
  setPen(pen);
}
