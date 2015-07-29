#include "GraphicsPointItem.h"

#include "GraphicsLineItem.h"
#include "MeshEditor.h"

#include <QPen>

double GraphicsPointItem::SIZE = 0.375;

GraphicsPointItem::GraphicsPointItem(const flat::Point2& pos, QGraphicsItem *parent):
    QGraphicsEllipseItem(parent), mInputLine(nullptr), mOutputLine(nullptr) {
  setFlatPoint(pos);

  setFlag(QGraphicsItem::ItemIsSelectable);
  setAcceptHoverEvents(true);

  QPen pen(Qt::black, 2);
  pen.setCosmetic(true);
  setPen(pen);

  setHighlighted(false);

  //setBoundingRegionGranularity();
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

  QPointF position = MeshEditor::mapFromFlat(point);
  position.rx() -= SIZE / 2.0;
  position.ry() -= SIZE / 2.0;

  setRect(QRectF(position, QSizeF(SIZE, SIZE)));

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
