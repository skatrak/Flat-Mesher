#include "GraphicsLineItem.h"

#include "Configuration.h"
#include "GraphicsPointItem.h"
#include "MeshEditor.h"

#include <FlatMesher/Line2.h>

#include <QPen>
#include <QStyleOptionGraphicsItem>

GraphicsLineItem::GraphicsLineItem(GraphicsPointItem *src, GraphicsPointItem *dest,
                                   QGraphicsItem *parent):
    QGraphicsLineItem(parent), mColor(Qt::black), mSrc(nullptr), mDest(nullptr) {
  setSrc(src);
  setDest(dest);

  setFlag(QGraphicsItem::ItemIsSelectable);
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
  setPen(QPen(mColor, cellSize / 8));
}

QVariant GraphicsLineItem::itemChange(GraphicsItemChange change, const QVariant& value) {
  if (change == ItemSelectedHasChanged) {
    mColor = value.toBool()? QColor(204, 128, 14) : Qt::black;
    cellSizeChanged(pen().widthF() * 8);
  }

  return QGraphicsItem::itemChange(change, value);
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget) {
  QStyleOptionGraphicsItem opt = *option;
  opt.state &= ~QStyle::State_Selected;
  QGraphicsLineItem::paint(painter, &opt, widget);
}
