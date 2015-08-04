#include "MeshEditor.h"

#include "Configuration.h"
#include "EditorCommands.h"
#include "GraphicsLineItem.h"
#include "GraphicsPointItem.h"
#include "GridGraphicsView.h"

#include <FlatMesher/Utils.h>

#include <QGridLayout>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QUndoStack>

#include <cmath>
#include <vector>

MeshEditor::MeshEditor(QWidget *parent): QWidget(parent),
    mTriangleSize(config::DEFAULT_TRIANGLE_SZ), mWallsHeight(config::DEFAULT_WALLS_HEIGHT),
    mFirstPoint(nullptr), mPointsAmount(0), mMousePressed(false), mMouseMoved(false),
    mFirstSelectedMoved(nullptr) {
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

  connect(mScene,                       SIGNAL(selectionChanged()),    this, SLOT(onSelectionChanged()));
  connect(mView,                        SIGNAL(mouseMoved(QPoint)),    this, SLOT(onMouseMoved(QPoint)));
  connect(mView,                        SIGNAL(mousePressed(QPoint)),  this, SLOT(onMousePressed(QPoint)));
  connect(mView,                        SIGNAL(mouseReleased(QPoint)), this, SLOT(onMouseReleased(QPoint)));
  connect(mView->horizontalScrollBar(), SIGNAL(valueChanged(int)),     this, SLOT(onScrollBarMoved()));
  connect(mView->verticalScrollBar(),   SIGNAL(valueChanged(int)),     this, SLOT(onScrollBarMoved()));
}

MeshEditor::MeshEditor(const QString& fileName, const flat::FloorPlan& plan, QWidget *parent):
    MeshEditor(parent) {
  setFileName(fileName);
  loadPlan(plan);
}

flat::FloorPlan MeshEditor::plan() const {
  std::vector<flat::Point2> points;

  GraphicsPointItem* point = mFirstPoint;
  if (point) {
    do {
      points.push_back(point->flatPoint());
      if (!point->outputLine())
        break;

      point = point->outputLine()->dest();
    } while (point && point != mFirstPoint);
  }

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
  return mPointsAmount;
}

SelectedItems MeshEditor::selectionType() const {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  GraphicsPointItem *point;

  switch (selected.size()) {
  case 0:
    return SelectedItems::None;
  case 1:
    point = dynamic_cast<GraphicsPointItem*>(selected.first());
    return point == nullptr? SelectedItems::Line : SelectedItems::Point;
  default:
    return SelectedItems::PointSet;
  }
}

flat::Point2 MeshEditor::selectedPoint() {
  GraphicsPointItem *point = selectedPointItem();
  return point? point->flatPoint() : flat::Point2();
}

QList<flat::Point2> MeshEditor::selectedPoints() {
  QList<GraphicsPointItem*> selected = selectedPointItems();

  QList<flat::Point2> result;
  result.reserve(selected.size());

  for (GraphicsPointItem *i: selected)
    result << i->flatPoint();

  return result;
}

flat::Line2 MeshEditor::selectedLine() {
  GraphicsLineItem *line = selectedLineItem();
  return line? mapToFlat(line->line()) : flat::Line2();
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

QPointF MeshEditor::snapToGrid(const QPointF& point, double triangleSize) {
  double x = std::round(point.x() / triangleSize) * triangleSize;
  double y = std::round(point.y() / triangleSize) * triangleSize;

  return QPointF(x, y);
}

void MeshEditor::setFileName(const QString& fileName) {
  mFileName = fileName;
}

void MeshEditor::setTrianglesSize(double triangleSize) {
  if (triangleSize != mTriangleSize)
    mUndoStack->push(new ChangeTriangleSizeCommand(this, triangleSize));
}

void MeshEditor::setWallsHeight(double wallsHeight) {
  if (wallsHeight != mWallsHeight)
    mUndoStack->push(new ChangeWallsHeightCommand(this, wallsHeight));
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
  // FIXME If you add points far away and then delete them, when re-setting the
  // FIXME viewport it will contain that points
  // TODO Handle manually the sceneRect size so that everything works as expected
  setViewport(mScene->sceneRect());
}

void MeshEditor::setGridVisible(bool visible) {
  mView->setGridVisible(visible);
}

void MeshEditor::setSelectionMode(SelectionMode mode) {
  if (mode != mCurrentMode) {
    switch (mode) {
    case SelectionMode::Selection:
      setCursor(Qt::ArrowCursor);
      mView->setDragMode(QGraphicsView::RubberBandDrag);
      mView->setInteractive(true);
      break;
    case SelectionMode::Hand:
      mView->setDragMode(QGraphicsView::ScrollHandDrag);
      mView->setInteractive(false);
      break;
    case SelectionMode::AddPoints:
      setCursor(Qt::CrossCursor);
      mView->setDragMode(QGraphicsView::NoDrag);
      mView->setInteractive(false);
      break;
    }

    mCurrentMode = mode;
  }
}

void MeshEditor::changeSelectedPoint(const flat::Point2& point) {
  movePoint(selectedPointItem(), point);
}

void MeshEditor::deleteSelectedPoints() {
  mUndoStack->push(new DeletePointsCommand(this, selectedPointItems()));
}

void MeshEditor::splitSelectedLine() {
  splitLine(selectedLineItem());
}

void MeshEditor::selectAllPoints() {
  GraphicsPointItem* point = mFirstPoint;
  if (point) {
    do {
      point->setSelected(true);
      if (!point->outputLine())
        break;

      point->outputLine()->setSelected(false);
      point = point->outputLine()->dest();
    } while (point && point != mFirstPoint);
  }
}

void MeshEditor::invertPointsOrder() {
  mUndoStack->push(new InvertOrderCommand(this));
}

void MeshEditor::onSelectionChanged() {
  emit selectionChanged(selectionType());
}

void MeshEditor::onMouseMoved(const QPoint& pos) {
  if (mMousePressed && !mMouseMoved) {
    GraphicsPointItem* point = mFirstPoint;
    if (point) {
      do {
        GraphicsLineItem* line = point->outputLine();
        if (!line)
          break;

        line->setFlag(QGraphicsItem::ItemIsSelectable, false);
        point = line->dest();
      } while (point && point != mFirstPoint);
    }

    mMouseMoved = true;
  }

  emit cursorMoved(mapToFlat(mView->mapToScene(pos)));
}

void MeshEditor::onMousePressed(const QPoint& /*pos*/) {
  mMousePressed = true;
}

void MeshEditor::onMouseReleased(const QPoint& pos) {
  GraphicsPointItem* point = mFirstPoint;

  switch (mCurrentMode) {
  case SelectionMode::AddPoints:
    appendPoint(mapToFlat(snapToGrid(mView->mapToScene(pos), mTriangleSize)));
    break;
  case SelectionMode::Selection:
    if (point && mMouseMoved) {
      do {
        GraphicsLineItem* line = point->outputLine();
        if (!line)
          break;

        line->setFlag(QGraphicsItem::ItemIsSelectable, true);
        point = line->dest();
      } while (point && point != mFirstPoint);
    }
    if (mFirstSelectedMoved) {
      flat::Point2 actPoint = mFirstSelectedMoved->flatPoint();
      QList<GraphicsPointItem*> movedPoints = selectedPointItems();

      mUndoStack->push(new MovePointsCommand(this, movedPoints,
                                             actPoint - mFirstSelectedPrevPosition));
      if (movedPoints.length() == 1)
        emit selectedPointMoved(actPoint);
    }
    break;
  default:
    break;
  }

  mFirstSelectedMoved = nullptr;
  mMousePressed = mMouseMoved = false;
}

void MeshEditor::onScrollBarMoved() {
  emit viewportChanged(viewport());
}

void MeshEditor::onPointDragged(GraphicsPointItem *point, const flat::Point2& oldPos) {
  if (!mFirstSelectedMoved) {
    mFirstSelectedMoved = point;
    mFirstSelectedPrevPosition = oldPos;
  }
}

void MeshEditor::setPlan(const flat::FloorPlan& plan) {
  mUndoStack->clear();

  mTriangleSize = plan.getTriangleSize();
  mView->setCellsSize(mTriangleSize);

  std::vector<flat::Point2> points = plan.getNodes();
  removePoints();

  if (points.size() > 2) {
    GraphicsPointItem *prevPoint = new GraphicsPointItem(points[0]);
    prevPoint->cellSizeChanged(mTriangleSize);
    mScene->addItem(prevPoint);
    mFirstPoint = prevPoint;

    ++mPointsAmount;

    for (unsigned i = 1; i < points.size(); ++i) {
      GraphicsPointItem *actPoint = new GraphicsPointItem(points[i]);
      GraphicsLineItem *line = new GraphicsLineItem(prevPoint, actPoint);

      actPoint->cellSizeChanged(mTriangleSize);
      line->cellSizeChanged(mTriangleSize);

      mScene->addItem(line);
      mScene->addItem(actPoint);

      prevPoint->setOutputLine(line);
      actPoint->setInputLine(line);

      ++mPointsAmount;
      prevPoint = actPoint;
    }

    GraphicsLineItem *line = new GraphicsLineItem(prevPoint, mFirstPoint);
    line->cellSizeChanged(mTriangleSize);
    mScene->addItem(line);

    prevPoint->setOutputLine(line);
    mFirstPoint->setInputLine(line);

    prevPoint->setHighlight(HighlightMode::Last);
    mFirstPoint->setHighlight(HighlightMode::First);
  }

  emit pointsAmountChanged(pointCount());
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

  if (!flat::utils::areEqual(zoomX, 1.0) || !flat::utils::areEqual(zoomY, 1.0))
    emit viewportChanged(mapToFlat(sceneRect));
}

void MeshEditor::removePoints() {
  while (mPointsAmount > 0) {
    Q_ASSERT(mFirstPoint != nullptr);

    GraphicsPointItem *next = nullptr;
    if (mFirstPoint->outputLine())
      next = mFirstPoint->outputLine()->dest();

    delete mFirstPoint;
    mFirstPoint = next;
    --mPointsAmount;
  }

  mFirstPoint = nullptr;
  Q_ASSERT(mPointsAmount == 0);

  emit pointsAmountChanged(mPointsAmount);
}

GraphicsPointItem* MeshEditor::selectedPointItem() {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  if (selected.isEmpty() || selected.size() > 1)
    return nullptr;

  return dynamic_cast<GraphicsPointItem*>(selected.first());
}

QList<GraphicsPointItem*> MeshEditor::selectedPointItems() {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  QList<GraphicsPointItem*> points;

  for (QGraphicsItem *i: selected) {
    if (GraphicsPointItem *point = dynamic_cast<GraphicsPointItem*>(i))
      points << point;
  }

  return points;
}

GraphicsLineItem* MeshEditor::selectedLineItem() {
  QList<QGraphicsItem*> selected = mScene->selectedItems();
  if (selected.isEmpty() || selected.size() > 1)
    return nullptr;

  return dynamic_cast<GraphicsLineItem*>(selected.first());
}

void MeshEditor::appendPoint(const flat::Point2& point) {
  mUndoStack->push(new AppendPointCommand(this, point));
}

void MeshEditor::deletePoint(GraphicsPointItem *point) {
  mUndoStack->push(new DeletePointsCommand(this, {point}));
}

void MeshEditor::movePoint(GraphicsPointItem *point, const flat::Point2& pos) {
  if (point) {
    flat::Point2 lastPos = point->flatPoint();
    point->setFlatPoint(pos);
    mUndoStack->push(new MovePointsCommand(this, {point}, pos - lastPos));
  }
}

void MeshEditor::splitLine(GraphicsLineItem *line) {
  mUndoStack->push(new SplitLineCommand(this, line));
}

void MeshEditor::_setTriangleSize(double triangleSize) {
  if (triangleSize != mTriangleSize) {
    mTriangleSize = triangleSize;
    mView->setCellsSize(triangleSize);

    emit triangleSizeChanged(triangleSize);
  }
}

void MeshEditor::_setWallsHeight(double wallsHeight) {
  if (wallsHeight != mWallsHeight) {
    mWallsHeight = wallsHeight;

    emit wallsHeightChanged(wallsHeight);
  }
}

void MeshEditor::setPointsAmount(int amount) {
  if (mPointsAmount != amount) {
    mPointsAmount = amount;
    emit pointsAmountChanged(mPointsAmount);
  }
}
