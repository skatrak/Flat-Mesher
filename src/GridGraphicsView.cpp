#include "GridGraphicsView.h"

#include "Configuration.h"

#include <cmath>

GridGraphicsView::GridGraphicsView(QWidget* parent):
    QGraphicsView(parent), mCellsSize(config::DEFAULT_TRIANGLE_SZ),
    mGridVisible(true) {}

GridGraphicsView::GridGraphicsView(QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent), mCellsSize(config::DEFAULT_TRIANGLE_SZ),
    mGridVisible(true) {}

void GridGraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
  // FIXME For some reason there are too many lines created and only some
  // FIXME are visible, while the application updates very slowly
  if (mGridVisible) {
    painter->save();
    painter->setPen(QPen(Qt::black, mCellsSize / 50.0, Qt::DotLine));

    double left = std::ceil(rect.left() / mCellsSize) * mCellsSize;
    double top = std::ceil(rect.top() / mCellsSize) * mCellsSize;

    //  for (double x = left; x < rect.right(); x += mCellsSize)
    //    for (double y = top; y < rect.bottom(); y += mCellsSize)
    //      painter->drawPoint(x, y);

    QVarLengthArray<QLineF> lines;

    for (qreal x = left; x < rect.right(); x += mCellsSize)
      lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += mCellsSize)
      lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(lines.data(), lines.size());
    painter->restore();
  }
}
