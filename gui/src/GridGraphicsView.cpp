#include "GridGraphicsView.h"

#include "Configuration.h"
#include "GraphicsLineItem.h"
#include "GraphicsPointItem.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <cmath>

GridGraphicsView::GridGraphicsView(QWidget* parent):
    QGraphicsView(parent), mCellsSize(config::DEFAULT_TRIANGLE_SZ),
    mGridVisible(true) {}

GridGraphicsView::GridGraphicsView(QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent), mCellsSize(config::DEFAULT_TRIANGLE_SZ),
    mGridVisible(true) {}

void GridGraphicsView::setCellsSize(double size) {
  if (size > 0.0 && size != mCellsSize) {
    mCellsSize = size;

    resetCachedContent();
    if (QGraphicsScene *scn = scene()) {
      scn->update();
      for (QGraphicsItem *item: scn->items()) {
        if (GraphicsPointItem *point = dynamic_cast<GraphicsPointItem*>(item))
          point->cellSizeChanged(mCellsSize);
        else if (GraphicsLineItem *line = dynamic_cast<GraphicsLineItem*>(item))
          line->cellSizeChanged(mCellsSize);
      }
    }
  }
}

void GridGraphicsView::setGridVisible(bool visible) {
  if (visible != mGridVisible) {
    mGridVisible = visible;

    resetCachedContent();
    if (QGraphicsScene *scn = scene())
      scn->update();
  }
}

void GridGraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
  if (mGridVisible) {
    painter->setPen(QPen(QColor(128, 128, 128, 64), 0));

    double left = std::ceil(rect.left() / mCellsSize) * mCellsSize;
    double top = std::ceil(rect.top() / mCellsSize) * mCellsSize;

    QVarLengthArray<QLineF> lines;
    for (qreal x = left; x < rect.right(); x += mCellsSize)
      lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += mCellsSize)
      lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(lines.data(), lines.size());
  }
}

void GridGraphicsView::wheelEvent(QWheelEvent* event) {
  QWheelEvent *wheelEvent = (QWheelEvent*) event;

  if (wheelEvent->modifiers().testFlag(Qt::ControlModifier)) {
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    double scaleFactor;
    if (wheelEvent->delta() > 0)
      scaleFactor = 1.15;
    else
      scaleFactor = 1.0 / 1.15;

    scale(scaleFactor, scaleFactor);
    emit zoomChanged(scaleFactor);
  }
  else
    QGraphicsView::wheelEvent(event);
}

void GridGraphicsView::mouseMoveEvent(QMouseEvent *event) {
  QGraphicsView::mouseMoveEvent(event);
  emit mouseMoved(event->pos());
}

void GridGraphicsView::mousePressEvent(QMouseEvent *event) {
  QGraphicsView::mousePressEvent(event);
  emit mousePressed(event->pos());
}

void GridGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
  QGraphicsView::mouseReleaseEvent(event);
  emit mouseReleased(event->pos());
}
