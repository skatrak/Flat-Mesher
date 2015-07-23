#include "MeshEditor.h"

#include "GridGraphicsView.h"

#include <QGridLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QUndoStack>

#include <vector>

MeshEditor::MeshEditor(QWidget *parent): QWidget(parent) {
  mUndoStack = new QUndoStack(this);
  mCurrentMode = SelectionMode::Selection;

  QGridLayout *layout = new QGridLayout(this);

  mScene = new QGraphicsScene(parent);
  mView = new GridGraphicsView(mScene, this);

  layout->addWidget(mView, 0, 0, 1, 1);

  mScene->addRect(0, 0.5, 0.2, 0.1);
  mScene->addRect(5, -0.1, 0.2, 0.1);
  //mView->scale(100, 100);
  adjustViewport();

  mView->setDragMode(QGraphicsView::RubberBandDrag);
}

MeshEditor::MeshEditor(const QString& fileName, const flat::FloorPlan& plan, QWidget *parent):
    MeshEditor(parent) {
  loadPlan(fileName, plan);
}

void MeshEditor::setFileName(const QString& fileName) {
  mFileName = fileName;
}

void MeshEditor::loadPlan(const QString& fileName, const flat::FloorPlan& plan) {
  setFileName(fileName);
  setPlan(plan);
}

void MeshEditor::setSaved() {
  mUndoStack->setClean();
}

flat::FloorPlan MeshEditor::plan() const {

}

flat::FlatMesh MeshEditor::mesh() const {
  flat::FloorPlan floor = plan();
  flat::FlatMesh mesh;

  mesh.createFromPlan(&floor);
  return mesh;
}

int MeshEditor::pointCount() const {

}

QUndoStack* MeshEditor::undoStack() const {
  return mUndoStack;
}

flat::Rectangle MeshEditor::viewport() const {
  return mapToFlat(mView->mapToScene(mView->rect()).boundingRect());
}

flat::Point2 MeshEditor::mapToFlat(const QPointF& point) {
  return flat::Point2(point.x(), -point.y());
}

flat::Rectangle MeshEditor::mapToFlat(const QRectF &rect) {
  return flat::Rectangle(-rect.bottom(), -rect.top(), rect.left(), rect.right());
}

QPointF MeshEditor::mapFromFlat(const flat::Point2& point) {
  return QPointF(point.getX(), -point.getY());
}

QRectF MeshEditor::mapFromFlat(const flat::Rectangle& rect) {
  flat::Point2 tl = rect.getTopLeft();
  flat::Point2 lr = rect.getLowerRight();

  return QRectF(tl.getX(), -lr.getY(), rect.getWidth(), -rect.getHeight());
}

void MeshEditor::setPlan(const flat::FloorPlan& plan) {
  QPen pen(Qt::black, 0.05);
  QSizeF size(0.1, 0.1);
  std::vector<flat::Point2> points = plan.getNodes();

  // FIXME It seems like the points are not connected properly
  for (int i = 0; i < points.size() - 1; ++i) {
    QGraphicsEllipseItem *point = mScene->addEllipse(QRectF(mapFromFlat(points[i]), size), pen);
    point->setAcceptedMouseButtons(Qt::LeftButton);
    point->setAcceptHoverEvents(true);

    QPointF next = mapFromFlat(points[i+1]);
    QGraphicsLineItem *line = mScene->addLine(QLineF(point->pos(), next), pen);
    line->setAcceptedMouseButtons(Qt::LeftButton);
    line->setAcceptHoverEvents(true);
  }

  adjustViewport();
}

// FIXME This is not working properly (not enough zoom)
void MeshEditor::adjustViewport() {
  QRectF sceneRect = mScene->sceneRect();
  QRectF viewRect = mView->mapToScene(mView->rect()).boundingRect();

  double zoomX = viewRect.width() / sceneRect.width();
  double zoomY = viewRect.height() / sceneRect.height();

  mView->scale(zoomX, zoomY);
  mView->update();
}

void MeshEditor::setViewport(const flat::Rectangle& viewport) {
  // FIXME This approach is only useful when the scene is bigger than the view
  mView->fitInView(mScene->sceneRect());
}

void MeshEditor::setGridVisible(bool visible) {
  mView->setGridVisible(visible);
}

void MeshEditor::setGeneralProperties(double triangleSize, double wallsHeight) {
  mTriangleSize = triangleSize;
  mWallsHeight = wallsHeight;

  mView->setCellsSize(mTriangleSize);
}
