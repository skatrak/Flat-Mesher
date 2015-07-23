#ifndef GRIDGRAPHICSVIEW_H
#define GRIDGRAPHICSVIEW_H

#include <QGraphicsView>

class GridGraphicsView: public QGraphicsView {
  Q_OBJECT

public:
  GridGraphicsView(QWidget *parent = 0);
  GridGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
  virtual ~GridGraphicsView() = default;

  double cellsSize() const { return mCellsSize; }
  bool isGridVisible() const { return mGridVisible; }

public slots:
  void setCellsSize(double size) { if (size > 0.0) mCellsSize = size; }
  void setGridVisible(bool visible) { mGridVisible = visible; update(); }

protected:
  void drawBackground(QPainter *painter, const QRectF &rect);

private:
  double mCellsSize;
  bool mGridVisible;

};

#endif // GRIDGRAPHICSVIEW_H
