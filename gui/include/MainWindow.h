#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SelectedItems.h"
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
class QPushButton;
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
  void selectAll();

  // Tools
  void toolChanged(QAction *toolAction);
  void inspectMesh();
  void invertPoints();

  // Help
  void about();

protected slots:
  void onTriangleSizeChanged(double triangleSize);
  void onWallsHeightChanged(double wallsHeight);
  void onCursorMoved(const flat::Point2& pos);
  void onViewportChanged(const flat::Rectangle& viewport);
  void onPointsAmountChanged(int amount);
  void onSelectionChanged(SelectedItems selectionType);
  void onSavedChanged(bool saved);
  void onSelectedPointMoved(const flat::Point2& point);

protected:
  virtual void closeEvent(QCloseEvent *event);

  int findOpenFile(const QString& fileName) const;
  void configureAndSelectEditor(MeshEditor *editor, const QString& tabName);

private slots:
  void onTabChanged(int tabIndex);
  void onTabClose(int tabIndex);
  void onGridVisibilityChanged(bool visible);

  void onGeneralApplyClicked();
  void onViewportApplyClicked();
  void onViewportResetClicked();
  void onPointMoveClicked();
  void onPointDeleteClicked();
  void onSnapToGridClicked();
  void onLineSplitClicked();

private:
  bool saveEditorModel(int tabIndex);
  bool closeEditorModel(int tabIndex);
  void resetInputsToDefault();
  void editorAvailable(bool available);
  void setupActions();
  void setupPropertiesSidebar();
  void setupStatusBar();

  Ui::MainWindow *ui;

  QUndoGroup *mUndoGroup;
  MeshEditor *mCurrentEditor;
  SelectionMode mCurrentMode;

  // Toolbox actions
  QActionGroup *mToolActions;
  QAction *mActionSelectionTool, *mActionHandTool, *mActionAddTool;

  // Properties panel
  QDoubleSpinBox *mTriangleSz, *mWallsHeight;
  ViewportControls *mViewport;

  CollapsibleWidget *mSelectionCollapsible;
  QWidget *mSelectionPoint, *mSelectionLine;
  QDoubleSpinBox *mPointX, *mPointY;
  QPushButton *mPointMove;
  QLabel *mLineLength, *mLineSlope;

  // Status bar
  QLabel *mCursorPos, *mPointsCounter;

};

#endif // MAINWINDOW_H
