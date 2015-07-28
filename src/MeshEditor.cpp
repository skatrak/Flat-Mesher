#include "MeshEditor.h"

#include "Configuration.h"
#include "GridGraphicsView.h"

#include <QGridLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QUndoStack>

#include <vector>

MeshEditor::MeshEditor(QWidget *parent): QWidget(parent),
    mTriangleSize(config::DEFAULT_TRIANGLE_SZ), mWallsHeight(config::DEFAULT_WALLS_HEIGHT) {
  mUndoStack = new QUndoStack(this);
  mCurrentMode = SelectionMode::Selection;

  mScene = new QGraphicsScene(this);
  mView = new GridGraphicsView(mScene, this);

  mView->setInteractive(true);
  mView->setDragMode(QGraphicsView::RubberBandDrag);
  mView->setMouseTracking(true);

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(mView, 0, 0, 1, 1);

  flat::Rectangle vp(config::DEFAULT_VIEWPORT_MAX_Y, config::DEFAULT_VIEWPORT_MIN_Y,
                     config::DEFAULT_VIEWPORT_MIN_X, config::DEFAULT_VIEWPORT_MAX_X);
  setViewport(vp);

  //connect(mScene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(onSceneRectChanged(QRectF)));
  connect(mScene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  connect(mView, SIGNAL(mouseMoved(QPoint)), this, SLOT(onMouseMoved(QPoint)));
}

MeshEditor::MeshEditor(const QString& fileName, const flat::FloorPlan& plan, QWidget *parent):
    MeshEditor(parent) {
  setFileName(fileName);
  loadPlan(plan);
}

flat::FloorPlan MeshEditor::plan() const {
  std::vector<flat::Point2> points;
  for (QGraphicsItem* i: mPointsList)
    points.push_back(mapToFlat(i->data(0).toPointF()));

  flat::FloorPlan plan;
  plan.setTriangleSize(mTriangleSize);
  plan.setHeight(mWallsHeight);
  plan.setNodes(points);

  return plan;
}

flat::FlatMesh MeshEditor::mesh() const {
  flat::FloorPlan floor = plan();
  flat::FlatMesh mesh;

  mesh.createFromPlan(&floor);
  return mesh;
}

flat::Rectangle MeshEditor::viewport() const {
  return mapToFlat(mView->mapToScene(mView->rect()).boundingRect());
}

int MeshEditor::pointCount() const {
  return mPointsList.size();
}

SelectedItems MeshEditor::selectionType() const {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  QGraphicsEllipseItem *ellipse;

  switch (selected.size()) {
  case 0:
    return SelectedItems::None;
  case 1:
    ellipse = dynamic_cast<QGraphicsEllipseItem*>(selected.first());
    return ellipse == nullptr? SelectedItems::Line : SelectedItems::Point;
  default:
    return SelectedItems::PointSet;
  }
}

flat::Point2 MeshEditor::selectedPoint() const {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  if (selected.size() != 1)
    return flat::Point2();

  QGraphicsEllipseItem *point = dynamic_cast<QGraphicsEllipseItem*>(selected.first());
  if (point == nullptr)
    return flat::Point2();

  return mapToFlat(point->data(0).toPointF());
}

QList<flat::Point2> MeshEditor::selectedPoints() const {
  QList<flat::Point2> result;
  QList<QGraphicsItem*> selected = mScene->selectedItems();

  for (QGraphicsItem* i: selected) {
    QGraphicsEllipseItem *ellipse = dynamic_cast<QGraphicsEllipseItem*>(i);
    if (ellipse != nullptr)
      result.append(mapToFlat(ellipse->data(0).toPointF()));
  }

  return result;
}

flat::Line2 MeshEditor::selectedLine() const {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  if (selected.size() != 1)
    return flat::Line2();

  QGraphicsLineItem *line = dynamic_cast<QGraphicsLineItem*>(selected.first());
  if (line == nullptr)
    return flat::Line2();

  return mapToFlat(line->line());
}

bool MeshEditor::isGridVisible() const {
  return mView->isGridVisible();
}

bool MeshEditor::isSaved() const {
  return mUndoStack->isClean();
}

flat::Point2 MeshEditor::mapToFlat(const QPointF& point) {
  return flat::Point2(point.x(), -point.y());
}

flat::Rectangle MeshEditor::mapToFlat(const QRectF &rect) {
  return flat::Rectangle(-rect.top(), -rect.bottom(), rect.left(), rect.right());
}

flat::Line2 MeshEditor::mapToFlat(const QLineF& line) {
  return flat::Line2(mapToFlat(line.p1()), mapToFlat(line.p2()));
}

QPointF MeshEditor::mapFromFlat(const flat::Point2& point) {
  return QPointF(point.getX(), -point.getY());
}

QLineF MeshEditor::mapFromFlat(const flat::Line2 &line) {
  return QLineF(mapFromFlat(line.getA()), mapFromFlat(line.getB()));
}

QRectF MeshEditor::mapFromFlat(const flat::Rectangle& rect) {
  return QRectF(rect.getLeft(), -rect.getTop(), rect.getWidth(), rect.getHeight());
}

void MeshEditor::setFileName(const QString& fileName) {
  mFileName = fileName;
}

void MeshEditor::setTrianglesSize(double triangleSize) {
  mTriangleSize = triangleSize;
  mView->setCellsSize(mTriangleSize);
}

void MeshEditor::setWallsHeight(double wallsHeight) {
  mWallsHeight = wallsHeight;
}

void MeshEditor::loadPlan(const flat::FloorPlan& plan) {
  setPlan(plan);
}

void MeshEditor::setSaved() {
  mUndoStack->setClean();
}

void MeshEditor::setViewport(const flat::Rectangle& viewport) {
  setViewport(mapFromFlat(viewport));
}

void MeshEditor::adjustViewport() {
  setViewport(mScene->sceneRect());
}

void MeshEditor::setGridVisible(bool visible) {
  mView->setGridVisible(visible);
}

void MeshEditor::setSelectionMode(SelectionMode mode) {
  if (mode != mCurrentMode) {
    switch (mode) {
    case SelectionMode::Selection:
      mView->setDragMode(QGraphicsView::RubberBandDrag);
      mView->setInteractive(true);
      break;
    case SelectionMode::Hand:
      mView->setDragMode(QGraphicsView::ScrollHandDrag);
      mView->setInteractive(false);
      break;
    case SelectionMode::AddPoints:
      mView->setDragMode(QGraphicsView::NoDrag);
      mView->setInteractive(true);
      break;
    }

    mCurrentMode = mode;
  }
}

void MeshEditor::changeSelectedPoint(const flat::Point2& point) {

}

void MeshEditor::deleteSelectedPoints() {

}

void MeshEditor::splitSelectedLine() {

}

void MeshEditor::onSelectionChanged() {
  emit selectionChanged(selectionType());
}

void MeshEditor::setViewport(const QRectF& sceneRect) {
  // FIXME The view is always centered, so the viewport is only correct when
  // FIXME the scene doesn't fit in the view or it's just the right size.
  // FIXME When the view is bigger than the scene displayed, it's centered.
  QRectF viewRect = mView->mapToScene(mView->rect()).boundingRect();

  double zoomX = viewRect.width() / (sceneRect.width() + mTriangleSize);
  double zoomY = viewRect.height() / (sceneRect.height() + mTriangleSize);

  mView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
  mView->centerOn(sceneRect.center());
  mView->scale(zoomX, zoomY);

  mView->update();
  //emit viewportChanged(viewport());
}

void MeshEditor::onMouseMoved(const QPoint& pos) {
  emit cursorMoved(mapToFlat(mView->mapToScene(pos)));
}

void MeshEditor::setPlan(const flat::FloorPlan& plan) {
  int pointsAmount = pointCount();

  mTriangleSize = plan.getTriangleSize();
  mView->setCellsSize(mTriangleSize);

  QPen pen(Qt::black, 2);
  pen.setCosmetic(true);
  QBrush brush(Qt::blue);

  QSizeF size(0.25, 0.25);
  std::vector<flat::Point2> points = plan.getNodes();

  for (unsigned i = 0; i < points.size(); ++i) {
    QPointF current = mapFromFlat(points[i]);
    QPointF next = mapFromFlat(points[(i + 1) % points.size()]);

    QRectF box(current.x() - size.width() / 2.0,
               current.y() - size.height() / 2.0,
               size.width(), size.height());

    QGraphicsEllipseItem *point = mScene->addEllipse(box, pen, brush);
    point->setFlag(QGraphicsItem::ItemIsSelectable);
    point->setAcceptHoverEvents(true);
    point->setData(0, current);

    mPointsList.insert(point);

    QGraphicsLineItem *line = mScene->addLine(QLineF(current, next), pen);
    line->setFlag(QGraphicsItem::ItemIsSelectable);
    line->setAcceptHoverEvents(true);
    line->setZValue(-1.0);
  }

  emit pointsAmountChanged(pointsAmount - pointCount());
}
