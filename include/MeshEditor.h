#ifndef MESHEDITOR_H
#define MESHEDITOR_H

#include <QList>
#include <QStack>
#include <QString>
#include <QWidget>

#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/Line2.h>
#include <FlatMesher/Point2.h>
#include <FlatMesher/Rectangle.h>

#include "SelectedItems.h"
#include "SelectionMode.h"

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
  flat::Point2 selectedPoint() const;
  QList<flat::Point2> selectedPoints() const;
  flat::Line2 selectedLine() const;

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

  void changeSelectedPoint(const flat::Point2& point);
  void deleteSelectedPoints();
  void splitSelectedLine();

signals:
  void cursorMoved(const flat::Point2& point);
  void viewportChanged(const flat::Rectangle& viewport);
  void pointsAmountChanged(int diff);
  void selectionChanged(SelectedItems selectionType);

protected slots:
  void onSelectionChanged();

protected:
  void setPlan(const flat::FloorPlan& plan);

private:
  QString mFileName;
  double mTriangleSize, mWallsHeight;

  QStack<QGraphicsItem*> mPointsStack;
  QUndoStack *mUndoStack;
  SelectionMode mCurrentMode;

  QGraphicsScene *mScene;
  GridGraphicsView *mView;

};

#endif // MESHEDITOR_H
