#include "GraphicsPointItem.h"

#include "Configuration.h"
#include "GraphicsLineItem.h"
#include "MeshEditor.h"

#include <QPen>
#include <QStyleOptionGraphicsItem>

GraphicsPointItem::GraphicsPointItem(const flat::Point2& pos, QGraphicsItem *parent):
    QObject(nullptr), QGraphicsEllipseItem(parent), mEdgesColor(Qt::black),
    mInputLine(nullptr), mOutputLine(nullptr) {
  setFlatPoint(pos);

  setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);

  cellSizeChanged(config::DEFAULT_TRIANGLE_SZ);
  setHighlight(HighlightMode::None);
}

GraphicsPointItem::~GraphicsPointItem() {
  QPair<GraphicsLineItem*, GraphicsLineItem*> lines = detach();
  GraphicsLineItem *in = lines.first, *out = lines.second;

  if (in && !in->src() && !in->dest())
    delete in;

  if (out && !out->src() && !out->dest())
    delete out;
}

bool GraphicsPointItem::isDetached() const {
  return !mInputLine && !mOutputLine;
}

GraphicsPointItem* GraphicsPointItem::nextPoint() {
  return mOutputLine? mOutputLine->dest() : nullptr;
}

GraphicsPointItem* GraphicsPointItem::prevPoint() {
  return mInputLine? mInputLine->src() : nullptr;
}

void GraphicsPointItem::setFlatPoint(const flat::Point2& point) {
  mPoint = mLastPoint = point;
  cellSizeChanged(rect().size().width() * 2);

  if (mInputLine)
    mInputLine->updateEnds();
  if (mOutputLine)
    mOutputLine->updateEnds();
}

void GraphicsPointItem::setInputLine(GraphicsLineItem *line) {
  mInputLine = line;
}

void GraphicsPointItem::setOutputLine(GraphicsLineItem *line) {
  mOutputLine = line;
}

void GraphicsPointItem::setHighlight(HighlightMode highlight) {
  switch (highlight) {
  case HighlightMode::None:
    setBrush(QBrush(QColor(50, 82, 179)));
    break;
  case HighlightMode::First:
    setBrush(QBrush(QColor(82, 179, 50)));
    break;
  case HighlightMode::Last:
    setBrush(QBrush(QColor(179, 50, 82)));
    break;
  }
}

void GraphicsPointItem::invertConnection() {
  GraphicsLineItem *tmp = mInputLine;
  mInputLine = mOutputLine;
  mOutputLine = tmp;
}

QPair<GraphicsLineItem*, GraphicsLineItem*> GraphicsPointItem::detach() {
  GraphicsLineItem *inL = mInputLine, *outL = mOutputLine;
  GraphicsPointItem *inP = prevPoint(), *outP = nextPoint();

  if (inP && outP) {
    outL->setSrc(nullptr);
    outL->setDest(nullptr);

    if (inP == outP) {
      inL->setSrc(nullptr);
      inL->setDest(nullptr);

      inP->mInputLine = inP->mOutputLine = nullptr;
    }
    else {
      outP->setInputLine(inL);
      inL->setDest(outP);
    }
  }

  mInputLine = mOutputLine = nullptr;
  return {inL, outL};
}

void GraphicsPointItem::cellSizeChanged(double cellSize) {
  QPen pen(mEdgesColor, cellSize / 10);
  setPen(pen);

  double sz = cellSize / 2;
  QPointF position = MeshEditor::mapFromFlat(mPoint);
  mInitialPos = position;
  setPos(0, 0);

  position.rx() -= sz / 2;
  position.ry() -= sz / 2;

  setRect(QRectF(position, QSizeF(sz, sz)));
  mCellSize = cellSize;
}

QVariant GraphicsPointItem::itemChange(GraphicsItemChange change, const QVariant& value) {
  if (scene()) {
    QPointF scenePoint;

    switch (change) {
    case ItemPositionChange:
      scenePoint = MeshEditor::snapToGrid(value.toPointF(), mCellSize);
      mPoint = MeshEditor::mapToFlat(mInitialPos + scenePoint);

      if (mInputLine)
        mInputLine->updateEnds();
      if (mOutputLine)
        mOutputLine->updateEnds();

      return scenePoint;
    case ItemPositionHasChanged:
      emit itemMoved(this, mLastPoint);
      mLastPoint = mPoint;

      break;
    case ItemSelectedHasChanged:
      mEdgesColor = value.toBool()? QColor(204, 128, 14) : Qt::black;
      cellSizeChanged(mCellSize);

      break;
    default:
      break;
    }
  }

  return QGraphicsItem::itemChange(change, value);
}

void GraphicsPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
  QStyleOptionGraphicsItem opt = *option;
  opt.state &= ~QStyle::State_Selected;
  QGraphicsEllipseItem::paint(painter, &opt, widget);
}
