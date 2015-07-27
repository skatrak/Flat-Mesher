#include "GridGraphicsView.h"

#include "Configuration.h"

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
    if (QGraphicsScene *scn = scene())
      scn->update();
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
    painter->save();
    painter->setPen(QPen(Qt::black, 0, Qt::DotLine));

    double left = std::ceil(rect.left() / mCellsSize) * mCellsSize;
    double top = std::ceil(rect.top() / mCellsSize) * mCellsSize;

    QVarLengthArray<QLineF> lines;
    for (qreal x = left; x < rect.right(); x += mCellsSize)
      lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += mCellsSize)
      lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(lines.data(), lines.size());
    painter->restore();
  }
}
