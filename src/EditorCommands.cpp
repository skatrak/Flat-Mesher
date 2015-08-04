#include "EditorCommands.h"

#include "GraphicsLineItem.h"
#include "GraphicsPointItem.h"
#include "MeshEditor.h"
#include "GridGraphicsView.h"

#include <QGraphicsScene>

// /////////////////////////////////////////////////////////////////////////////
// MeshEditorCommand
// /////////////////////////////////////////////////////////////////////////////

MeshEditorCommand::MeshEditorCommand(MeshEditor *editor, QUndoCommand *parent):
    QUndoCommand(parent), mEditor(editor) {}

void MeshEditorCommand::setTriangleSize(double triangleSize) {
  mEditor->mTriangleSize = triangleSize;
  mEditor->mView->setCellsSize(triangleSize);
}

void MeshEditorCommand::setWallsHeight(double wallsHeight) {
  mEditor->mWallsHeight = wallsHeight;
}

GraphicsPointItem* MeshEditorCommand::addPoint(const flat::Point2& point,
                                               GraphicsPointItem *prev) {
  GraphicsPointItem *newPoint = new GraphicsPointItem(point);
  insertPoint(newPoint, prev);

  return newPoint;
}

GraphicsLineItem* MeshEditorCommand::deletePoint(GraphicsPointItem *point) {
  GraphicsLineItem *line = extractPoint(point);

  if (point)
    delete point;

  if (!line->dest() && !line->src()) {
    delete line;
    line = nullptr;
  }

  return line;
}

void MeshEditorCommand::insertPoint(GraphicsPointItem *point, GraphicsPointItem *prev) {
  if (!point)
    return;

  if (!mEditor->mFirstPoint) {
    mEditor->mFirstPoint = point;
    point->setHighlight(HighlightMode::First);
  }
  else {
    if (mEditor->pointCount() == 1) {
      GraphicsLineItem *l1 = new GraphicsLineItem(mEditor->mFirstPoint, point);
      GraphicsLineItem *l2 = new GraphicsLineItem(point, mEditor->mFirstPoint);
      l1->cellSizeChanged(mEditor->mTriangleSize);
      l2->cellSizeChanged(mEditor->mTriangleSize);

      mEditor->mFirstPoint->setOutputLine(l1);
      mEditor->mFirstPoint->setInputLine(l2);
      point->setOutputLine(l2);
      point->setInputLine(l1);

      point->setHighlight(HighlightMode::Last);

      mEditor->mScene->addItem(l1);
      mEditor->mScene->addItem(l2);
    }
    else {
      GraphicsLineItem *iLine = prev? prev->outputLine() :
                                      mEditor->mFirstPoint->inputLine();

      if (iLine->dest() == mEditor->mFirstPoint) {
        iLine->src()->setHighlight(HighlightMode::None);
        point->setHighlight(HighlightMode::Last);
      }

      GraphicsLineItem* oLine = iLine->splitLine(point);
      oLine->cellSizeChanged(mEditor->mTriangleSize);

      mEditor->mScene->addItem(oLine);
    }
  }

  point->cellSizeChanged(mEditor->mTriangleSize);
  mEditor->mScene->addItem(point);

  mEditor->setPointsAmount(mEditor->mPointsAmount + 1);
}

GraphicsLineItem* MeshEditorCommand::extractPoint(GraphicsPointItem *point) {
  GraphicsLineItem *newLine = nullptr;
  if (point != nullptr) {
    if (point == mEditor->mFirstPoint) {
      if (point->outputLine()) {
        mEditor->mFirstPoint = point->nextPoint();
        mEditor->mFirstPoint->setHighlight(HighlightMode::First);
      }
      else
        mEditor->mFirstPoint = nullptr;
    }
    else if (point->nextPoint() == mEditor->mFirstPoint &&
             point->prevPoint() && point->prevPoint() != mEditor->mFirstPoint)
      point->prevPoint()->setHighlight(HighlightMode::Last);

    mEditor->mScene->removeItem(point);
    mEditor->setPointsAmount(mEditor->mPointsAmount - 1);

    // We return the input line because that's the one that may have been
    // redirected. The output line is deleted because it's not used anymore.
    QPair<GraphicsLineItem*, GraphicsLineItem*> lines = point->detach();
    if (lines.second)
      delete lines.second;

    newLine = lines.first;
  }

  return newLine;
}

void MeshEditorCommand::movePoints(QList<GraphicsPointItem*> pointItems,
                                   const flat::Point2& offset) {
  for (GraphicsPointItem *point: pointItems)
    if (point)
      point->setFlatPoint(point->flatPoint() + offset);
}

QPair<GraphicsPointItem*, GraphicsLineItem*> MeshEditorCommand::splitLine(GraphicsLineItem *line) {
  if (line) {
    QPair<GraphicsPointItem*, GraphicsLineItem*> pair = line->splitLine();
    GraphicsPointItem *point = pair.first;
    GraphicsLineItem *newLine = pair.second;

    point->cellSizeChanged(mEditor->mTriangleSize);
    newLine->cellSizeChanged(mEditor->mTriangleSize);

    if (point->outputLine()->dest() == mEditor->mFirstPoint) {
      point->setHighlight(HighlightMode::Last);
      point->inputLine()->src()->setHighlight(HighlightMode::None);
    }

    mEditor->mScene->addItem(newLine);
    mEditor->mScene->addItem(point);

    mEditor->setPointsAmount(mEditor->mPointsAmount + 1);
    return pair;
  }

  return QPair<GraphicsPointItem*, GraphicsLineItem*>(nullptr, nullptr);
}

void MeshEditorCommand::invertPointsOrder() {
  GraphicsPointItem* point = mEditor->mFirstPoint;
  if (point) {
    do {
      if (!point->outputLine())
        break;

      GraphicsPointItem* next = point->outputLine()->dest();
      point->outputLine()->invertConnection();
      point->invertConnection();
      point = next;
    } while (point && point != mEditor->mFirstPoint);

    if (mEditor->mFirstPoint->outputLine() && mEditor->mFirstPoint->inputLine()) {
      mEditor->mFirstPoint->outputLine()->dest()->setHighlight(HighlightMode::None);
      mEditor->mFirstPoint->inputLine()->src()->setHighlight(HighlightMode::Last);
    }
  }
}

// /////////////////////////////////////////////////////////////////////////////
// AppendPointCommand
// /////////////////////////////////////////////////////////////////////////////

AppendPointCommand::AppendPointCommand(MeshEditor *editor, const flat::Point2& point,
                                       QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mPosition(point), mPoint(nullptr) {
  setText(QObject::tr("Add point (%1, %2)").arg(point.getX()).arg(point.getY()));
}

void AppendPointCommand::undo() {
  deletePoint(mPoint);
  mPoint = nullptr;
}

void AppendPointCommand::redo() {
  mPoint = addPoint(mPosition);
}

// /////////////////////////////////////////////////////////////////////////////
// DeletePointsCommand
// /////////////////////////////////////////////////////////////////////////////

DeletePointsCommand::DeletePointsCommand(MeshEditor *editor,
                                         QList<GraphicsPointItem*> points,
                                         QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mPointItems(points) {

  for (GraphicsPointItem *point: mPointItems)
    mPoints << point->flatPoint();

  setText(QObject::tr("Delete %1 points").arg(mPoints.size()));
}

void DeletePointsCommand::undo() {
  // FIXME Points are inserted in the same place but not in the part of the
  // FIXME list of points they were before
  for (int i = 0; i < mPoints.length(); ++i)
    mPointItems[i] = addPoint(mPoints[i]);
}

void DeletePointsCommand::redo() {
  for (GraphicsPointItem *point: mPointItems)
    deletePoint(point);
}

// /////////////////////////////////////////////////////////////////////////////
// MovePointsCommand
// /////////////////////////////////////////////////////////////////////////////

MovePointsCommand::MovePointsCommand(MeshEditor *editor, QList<GraphicsPointItem*> points,
                                     const flat::Point2& oldFirstPos,
                                     QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mPoints(points) {

  mOldPos.reserve(mPoints.size());
  mOffset = mPoints[0]->flatPoint() - oldFirstPos;

  for (GraphicsPointItem* point: mPoints)
    mOldPos << point->flatPoint() - mOffset;

  setText(QObject::tr("Move %1 points").arg(mPoints.size()));
}

void MovePointsCommand::undo() {
  for (int i = 0; i < mPoints.size(); ++i)
    mPoints[i]->setFlatPoint(mOldPos[i]);
}

void MovePointsCommand::redo() {
  for (int i = 0; i < mPoints.size(); ++i)
    mPoints[i]->setFlatPoint(mOldPos[i] + mOffset);
}

bool MovePointsCommand::mergeWith(const QUndoCommand *other) {
  if (const MovePointsCommand *command = dynamic_cast<const MovePointsCommand*>(other)) {
    if (command->mPoints.size() != mPoints.size())
      return false;

    for (int i = 0; i < mPoints.size(); ++i)
      if (mPoints[i] != command->mPoints[i])
        return false;

    mOffset = mOffset + command->mOffset;
    return true;
  }

  return false;
}

// /////////////////////////////////////////////////////////////////////////////
// SplitLineCommand
// /////////////////////////////////////////////////////////////////////////////

SplitLineCommand::SplitLineCommand(MeshEditor *editor, GraphicsLineItem* line,
                                   QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mLine(line), mMiddlePoint(nullptr) {
  setText(QObject::tr("Split line"));
}

void SplitLineCommand::undo() {
  mLine = mMiddlePoint->inputLine();
  deletePoint(mMiddlePoint);
  mMiddlePoint = nullptr;
}

void SplitLineCommand::redo() {
  mMiddlePoint = splitLine(mLine).first;
}

// /////////////////////////////////////////////////////////////////////////////
// InvertOrderCommand
// /////////////////////////////////////////////////////////////////////////////

InvertOrderCommand::InvertOrderCommand(MeshEditor *editor, QUndoCommand *parent):
    MeshEditorCommand(editor, parent) {
  setText(QObject::tr("Invert points order"));
}

void InvertOrderCommand::undo() {
  invertPointsOrder();
}

void InvertOrderCommand::redo() {
  invertPointsOrder();
}

// /////////////////////////////////////////////////////////////////////////////
// ChangeTriangleSizeCommand
// /////////////////////////////////////////////////////////////////////////////

ChangeTriangleSizeCommand::ChangeTriangleSizeCommand(MeshEditor *editor,
                                                     double triangleSize,
                                                     QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mNewTriangleSize(triangleSize) {
  mOldTriangleSize = mEditor->triangleSize();
  setText(QObject::tr("Change triangle size (%1)").arg(triangleSize));
}

void ChangeTriangleSizeCommand::undo() {
  setTriangleSize(mOldTriangleSize);
}

void ChangeTriangleSizeCommand::redo() {
  setTriangleSize(mNewTriangleSize);
}

// /////////////////////////////////////////////////////////////////////////////
// ChangeWallsSizeCommand
// /////////////////////////////////////////////////////////////////////////////

ChangeWallsHeightCommand::ChangeWallsHeightCommand(MeshEditor *editor,
                                                   double wallsHeight,
                                                   QUndoCommand *parent):
    MeshEditorCommand(editor, parent), mNewWallsHeight(wallsHeight) {
  mOldWallsHeight = mEditor->wallsHeight();
  setText(QObject::tr("Change walls height (%1)").arg(wallsHeight));
}

void ChangeWallsHeightCommand::undo() {
  setWallsHeight(mOldWallsHeight);
}

void ChangeWallsHeightCommand::redo() {
  setWallsHeight(mNewWallsHeight);
}
