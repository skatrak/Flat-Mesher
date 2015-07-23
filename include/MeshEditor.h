#ifndef MESHEDITOR_H
#define MESHEDITOR_H

#include <QList>
#include <QString>
#include <QWidget>

#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/Line2.h>
#include <FlatMesher/Point2.h>
#include <FlatMesher/Rectangle.h>

#include "SelectionMode.h"

class GridGraphicsView;
class QGraphicsScene;
class QUndoStack;

class MeshEditor: public QWidget {
  Q_OBJECT

public:
  explicit MeshEditor(QWidget *parent = 0);
  MeshEditor(const QString& fileName, const flat::FloorPlan& plan, QWidget *parent = 0);

  void setFileName(const QString& fileName);
  void loadPlan(const QString& fileName, const flat::FloorPlan& plan);
  void setSaved();

  flat::FloorPlan plan() const;
  flat::FlatMesh mesh() const;
  int pointCount() const;
  QUndoStack* undoStack() const;
  flat::Rectangle viewport() const;
  QString fileName() const { return mFileName; }

  static flat::Point2 mapToFlat(const QPointF& point);
  static flat::Rectangle mapToFlat(const QRectF& rect);
  static QPointF mapFromFlat(const flat::Point2& point);
  static QRectF mapFromFlat(const flat::Rectangle& rect);

protected:
  void setPlan(const flat::FloorPlan& plan);

public slots:
  void adjustViewport();
  void setViewport(const flat::Rectangle& viewport);
  void setGridVisible(bool visible);
  void setGeneralProperties(double triangleSize, double wallsHeight);

signals:
  void selectionChanged(const flat::Point2& point);
  void selectionChanged(const QList<flat::Point2>& points);
  void selectionChanged(const flat::Line2& line);
  void selectionChanged();

  void pointsAdded(int pointCount);
  void pointsRemoved(int pointCount);
  void cursorMoved(const flat::Point2& point);

private:
  QString mFileName;

  double mTriangleSize, mWallsHeight;

  QGraphicsScene *mScene;
  GridGraphicsView *mView;

  QUndoStack *mUndoStack;
  SelectionMode mCurrentMode;

};

#endif // MESHEDITOR_H
