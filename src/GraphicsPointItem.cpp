#include "GraphicsPointItem.h"

#include "Configuration.h"
#include "GraphicsLineItem.h"
#include "MeshEditor.h"

#include <QPen>

GraphicsPointItem::GraphicsPointItem(const flat::Point2& pos, QGraphicsItem *parent):
    QGraphicsEllipseItem(parent), mInputLine(nullptr), mOutputLine(nullptr) {
  setFlatPoint(pos);

  setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);

  cellSizeChanged(config::DEFAULT_TRIANGLE_SZ);
  setHighlighted(false);
}

GraphicsPointItem::~GraphicsPointItem() {
  if (mInputLine && mOutputLine) {
    // Delete mOutputLine and redirect mInputLine to the second point
    // mOutputLine is connected
    GraphicsPointItem *next = mOutputLine->dest();
    delete mOutputLine;

    if (next) {
      if (next->outputLine() != mInputLine)
        next->setInputLine(mInputLine);
      else {
        delete mInputLine;
        return;
      }
    }

    mInputLine->setDest(next);
  }
}

void GraphicsPointItem::setFlatPoint(const flat::Point2& point) {
  mPoint = point;
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

void GraphicsPointItem::setHighlighted(bool highlighted) {
  setBrush(QBrush(highlighted? Qt::red : Qt::blue));
}

void GraphicsPointItem::invertConnection() {
  GraphicsLineItem *tmp = mInputLine;
  mInputLine = mOutputLine;
  mOutputLine = tmp;
}

void GraphicsPointItem::cellSizeChanged(double cellSize) {
  QPen pen(Qt::black, cellSize / 10);
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
  if (change == ItemPositionChange && scene()) {
    QPointF scenePoint = MeshEditor::snapToGrid(value.toPointF(), mCellSize);
    mPoint = MeshEditor::mapToFlat(mInitialPos + scenePoint);

    if (mInputLine)
      mInputLine->updateEnds();
    if (mOutputLine)
      mOutputLine->updateEnds();

    return scenePoint;
  }

  return QGraphicsItem::itemChange(change, value);
}
