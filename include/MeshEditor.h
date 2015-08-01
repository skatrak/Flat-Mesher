#ifndef MESHEDITOR_H
#define MESHEDITOR_H

#include <QSet>
#include <QString>
#include <QWidget>

#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/Line2.h>
#include <FlatMesher/Point2.h>
#include <FlatMesher/Rectangle.h>

#include "SelectedItems.h"
#include "SelectionMode.h"

class GraphicsLineItem;
class GraphicsPointItem;
class GridGraphicsView;
class QGraphicsItem;
class QGraphicsScene;
class QUndoStack;

class MeshEditor: public QWidget {
  Q_OBJECT

public:
  explicit MeshEditor(QWidget *parent = 0);
  MeshEditor(const QString& fileName, const flat::FloorPlan& plan, QWidget *parent = 0);

  flat::FloorPlan plan() const;
  flat::FlatMesh mesh() const;
  flat::Rectangle viewport() const;
  int pointCount() const;

  SelectedItems selectionType() const;
  flat::Point2 selectedPoint();
  QList<flat::Point2> selectedPoints();
  flat::Line2 selectedLine();

  bool isGridVisible() const;
  bool isSaved() const;

  QString fileName() const { return mFileName; }
  double triangleSize() const { return mTriangleSize; }
  double wallsHeight() const { return mWallsHeight; }
  QUndoStack* undoStack() const { return mUndoStack; }
  SelectionMode currentMode() const { return mCurrentMode; }

  static flat::Point2 mapToFlat(const QPointF& point);
  static flat::Line2 mapToFlat(const QLineF& line);
  static flat::Rectangle mapToFlat(const QRectF& rect);
  static QPointF mapFromFlat(const flat::Point2& point);
  static QLineF mapFromFlat(const flat::Line2 &line);
  static QRectF mapFromFlat(const flat::Rectangle& rect);
  static QPointF snapToGrid(const QPointF& point, double triangleSize);

public slots:
  void setFileName(const QString& fileName);
  void setTrianglesSize(double triangleSize);
  void setWallsHeight(double wallsHeight);

  void loadPlan(const flat::FloorPlan& plan);
  void setSaved();

  void setViewport(const flat::Rectangle& viewport);
  void adjustViewport();
  void setGridVisible(bool visible);
  void setSelectionMode(SelectionMode mode);

  void selectAllPoints();
  void invertPointsOrder();
  void addPoint(const flat::Point2& point);
  void changeSelectedPoint(const flat::Point2& point);
  void deleteSelectedPoints();
  void splitSelectedLine();

signals:
  void cursorMoved(const flat::Point2& point);
  void viewportChanged(const flat::Rectangle& viewport);
  void pointsAmountChanged(int amount);
  void selectionChanged(SelectedItems selectionType);

protected slots:
  void onSelectionChanged();
  void onMouseMoved(const QPoint& pos);
  void onMousePressed(const QPoint& pos);
  void onMouseReleased(const QPoint& pos);
  void onScrollBarMoved();

protected:
  void setPlan(const flat::FloorPlan& plan);
  void setViewport(const QRectF& sceneRect);
  void removePoints();

  GraphicsPointItem* selectedPointItem();
  QList<GraphicsPointItem*> selectedPointItems();
  GraphicsLineItem* selectedLineItem();

private:
  QString mFileName;
  double mTriangleSize, mWallsHeight;

  GraphicsPointItem* mFirstPoint;
  int mPointsAmount;

  QUndoStack *mUndoStack;
  SelectionMode mCurrentMode;

  QGraphicsScene *mScene;
  GridGraphicsView *mView;

  bool mMousePressed;

};

#endif // MESHEDITOR_H
