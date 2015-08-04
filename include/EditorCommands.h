#ifndef EDITORCOMMANDS
#define EDITORCOMMANDS

#include <QList>
#include <QUndoCommand>

#include <FlatMesher/Point2.h>

class GraphicsLineItem;
class GraphicsPointItem;
class MeshEditor;

class MeshEditorCommand: public QUndoCommand {
protected:
  explicit MeshEditorCommand(MeshEditor *editor, QUndoCommand *parent = 0);
  virtual ~MeshEditorCommand() = default;

  void setTriangleSize(double triangleSize);
  void setWallsHeight(double wallsHeight);

  GraphicsPointItem* addPoint(const flat::Point2& point, GraphicsPointItem *prev = 0);
  GraphicsLineItem* deletePoint(GraphicsPointItem *point);

  void insertPoint(GraphicsPointItem *point, GraphicsPointItem *prev = 0);
  GraphicsLineItem* extractPoint(GraphicsPointItem *point);

  void movePoints(QList<GraphicsPointItem*> pointItems, const flat::Point2& offset);
  QPair<GraphicsPointItem*, GraphicsLineItem*> splitLine(GraphicsLineItem *line);
  void invertPointsOrder();

  MeshEditor *mEditor;
};

class AppendPointCommand: public MeshEditorCommand {
public:
  AppendPointCommand(MeshEditor *editor, const flat::Point2& point,
                     QUndoCommand *parent = 0);
  virtual ~AppendPointCommand();

  virtual void undo();
  virtual void redo();

private:
  flat::Point2 mPosition;
  GraphicsPointItem *mPoint;

};

class DeletePointsCommand: public MeshEditorCommand {
public:
  DeletePointsCommand(MeshEditor *editor, QList<GraphicsPointItem*> points,
                      QUndoCommand *parent = 0);
  virtual ~DeletePointsCommand() = default;

  virtual void undo();
  virtual void redo();

private:
  QList<GraphicsPointItem*> mPoints;
  QList<GraphicsPointItem*> mPrevPoints;

};

class MovePointsCommand: public MeshEditorCommand {
public:
  MovePointsCommand(MeshEditor *editor, QList<GraphicsPointItem*> points,
                    const flat::Point2& oldFirstPos,
                    QUndoCommand *parent = 0);
  virtual ~MovePointsCommand() = default;

  virtual void undo();
  virtual void redo();
  virtual bool mergeWith(const QUndoCommand *other);
  virtual int id() const { return 1; }

private:
  flat::Point2 mOffset;
  QList<flat::Point2> mOldPos;
  QList<GraphicsPointItem*> mPoints;

};

class SplitLineCommand: public MeshEditorCommand {
public:
  explicit SplitLineCommand(MeshEditor *editor, GraphicsLineItem *line,
                            QUndoCommand *parent = 0);
  virtual ~SplitLineCommand();

  virtual void undo();
  virtual void redo();

private:
  GraphicsPointItem *mFirstPoint, *mSecondPoint, *mMiddlePoint;

};

class InvertOrderCommand: public MeshEditorCommand {
public:
  explicit InvertOrderCommand(MeshEditor *editor, QUndoCommand *parent = 0);
  virtual ~InvertOrderCommand() = default;

  virtual void undo();
  virtual void redo();

};

class ChangeTriangleSizeCommand: public MeshEditorCommand {
public:
  ChangeTriangleSizeCommand(MeshEditor *editor, double triangleSize,
                            QUndoCommand *parent = 0);
  virtual ~ChangeTriangleSizeCommand() = default;

  virtual void undo();
  virtual void redo();

private:
  double mOldTriangleSize, mNewTriangleSize;

};

class ChangeWallsHeightCommand: public MeshEditorCommand {
public:
  ChangeWallsHeightCommand(MeshEditor *editor, double wallsHeight,
                           QUndoCommand *parent = 0);
  virtual ~ChangeWallsHeightCommand() = default;

  virtual void undo();
  virtual void redo();

private:
  double mOldWallsHeight, mNewWallsHeight;

};

#endif // EDITORCOMMANDS
