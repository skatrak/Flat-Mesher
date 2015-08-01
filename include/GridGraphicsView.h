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
  void setCellsSize(double size);
  void setGridVisible(bool visible);

signals:
  void mouseMoved(const QPoint& pos);
  void mousePressed(const QPoint& pos);
  void mouseReleased(const QPoint& pos);

protected:
  void drawBackground(QPainter *painter, const QRectF &rect);

  virtual void wheelEvent(QWheelEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:
  double mCellsSize;
  bool mGridVisible;

};

#endif // GRIDGRAPHICSVIEW_H
