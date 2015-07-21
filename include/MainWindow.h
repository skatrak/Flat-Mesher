#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CollapsibleWidget;
class QActionGroup;
class QDoubleSpinBox;
class QLabel;

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  void createToolbarActions();
  void createPropertiesSidebar();
  void createStatusBar();

  Ui::MainWindow *ui;

  // Toolbox actions
  QActionGroup *mToolActions;
  QAction *mActionSelectionTool, *mActionHandTool, *mActionAddTool;
  QAction *mActionFindProblems;

  // Properties panel
  QDoubleSpinBox *mTriangleSz, *mWallsHeight;
  QDoubleSpinBox *mMinX, *mMaxX, *mMinY, *mMaxY;

  CollapsibleWidget *mSelectionCollapsible;
  QWidget *mSelectionPoint, *mSelectionLine;
  QDoubleSpinBox *mPointX, *mPointY;
  QLabel *mLineLength, *mLineSlope;

  // Status bar
  QLabel *mCursorPos, *mElementCounter;

};

#endif // MAINWINDOW_H
