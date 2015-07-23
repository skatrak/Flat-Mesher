#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SelectionMode.h"

namespace Ui {
class MainWindow;
}

namespace flat {
class Point2;
class Rectangle;
}

class CollapsibleWidget;
class MeshEditor;
class QActionGroup;
class QDoubleSpinBox;
class QLabel;
class QUndoGroup;
class ViewportControls;

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  // File
  void newFlat();
  void openFlat();

  void closeFlat();
  void closeAllFlats();

  void saveFlat();
  void saveFlatAs();
  void saveAllFlats();

  void exportMesh();

  // Edit
  void undo();
  void redo();

  // Tools
  void toolChanged(QAction *toolAction);
  void findProblems();

  // Help
  void about();

private slots:
  void generalApplyClicked();
  void viewportApplyClicked();
  void selectionApplyClicked();
  void tabChanged(int tabIndex);

signals:
  void changeGeneralProperties(double triangleSize, double wallsHeight);
  void changeViewPort(const flat::Rectangle& viewport);
  void moveSelectedPoint(const flat::Point2& point);
  void resetViewPort();
  void deleteSelectedPoints();
  void splitLine();
  void changeMode(SelectionMode mode);

private:
  void createToolbarActions();
  void createPropertiesSidebar();
  void createStatusBar();

  Ui::MainWindow *ui;

  QUndoGroup *mUndoGroup;
  MeshEditor *mCurrentEditor;
  SelectionMode mCurrentMode;

  // Toolbox actions
  QActionGroup *mToolActions;
  QAction *mActionSelectionTool, *mActionHandTool, *mActionAddTool;
  QAction *mActionFindProblems;

  // Properties panel
  QDoubleSpinBox *mTriangleSz, *mWallsHeight;
  ViewportControls *mViewport;

  CollapsibleWidget *mSelectionCollapsible;
  QWidget *mSelectionPoint, *mSelectionLine;
  QDoubleSpinBox *mPointX, *mPointY;
  QLabel *mLineLength, *mLineSlope;

  // Status bar
  QLabel *mCursorPos, *mElementCounter;

};

#endif // MAINWINDOW_H
