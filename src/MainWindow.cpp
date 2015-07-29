#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CollapsibleWidget.h"
#include "Configuration.h"
#include "FileManager.h"
#include "MeshEditor.h"
#include "ViewportControls.h"

#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>

#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QUndoGroup>
#include <QUndoStack>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow),
    mCurrentEditor(nullptr), mCurrentMode(SelectionMode::Selection) {
  ui->setupUi(this);

  setupActions();
  setupPropertiesSidebar();
  setupStatusBar();
  editorAvailable(false);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::newFlat() {
  configureAndSelectEditor(new MeshEditor(), tr("<unnamed>"));
}

void MainWindow::openFlat() {
  flat::FloorPlan plan;
  QString fileName = FileManager::openFlat(plan);

  if (!fileName.isNull()) {
    // If it is loaded, only swap the current tab
    int tabIndex = findOpenFile(fileName);
    if (tabIndex >= 0) {
      // Ask if user wants to replace unsaved data
      ui->planSet->setCurrentIndex(tabIndex);
      if (!mCurrentEditor->isSaved()) {
        int response = QMessageBox::question(this, tr("Replace changes"),
                                             tr("The current model has unsaved changes. Do you want to reload the model? All changes will be lost"),
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::No);
        if (response == QMessageBox::Yes)
          mCurrentEditor->loadPlan(plan);
      }
    }
    else {
      MeshEditor *editor = new MeshEditor();
      configureAndSelectEditor(editor, fileName.section('/', -1));

      editor->setFileName(fileName);
      editor->loadPlan(plan);
      editor->adjustViewport();
    }
  }
}

void MainWindow::closeFlat() {
  if (ui->planSet->count() > 0) {
    if (!mCurrentEditor->isSaved()) {
      int response = QMessageBox::question(this, tr("Save changes"),
                                           tr("The current model has unsaved changes. Do you want to save it?"),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                           QMessageBox::Save);
      switch (response) {
      case QMessageBox::Cancel:
        return;
      case QMessageBox::Save:
        FileManager::saveFlat(mCurrentEditor->plan(), mCurrentEditor->fileName());
      }
    }

    ui->planSet->removeTab(ui->planSet->currentIndex());
  }
}

void MainWindow::closeAllFlats() {
  while (ui->planSet->count() > 0)
    closeFlat();
}

void MainWindow::saveFlat() {
  saveEditorModel(ui->planSet->currentIndex());
}

void MainWindow::saveFlatAs() {
  if (mCurrentEditor != nullptr) {
    flat::FloorPlan plan = mCurrentEditor->plan();

    QString fileName = FileManager::saveFlat(plan);
    if (!fileName.isNull()) {
      QString file = fileName.section('/', -1);
      ui->planSet->tabBar()->setTabText(ui->planSet->currentIndex(), file);
    }
  }
}

void MainWindow::saveAllFlats() {
  for (int i = 0; i < ui->planSet->count(); ++i)
    saveEditorModel(i);
}

void MainWindow::exportMesh() {
  if (mCurrentEditor != nullptr)
    FileManager::saveMesh(mCurrentEditor->mesh());
}

void MainWindow::selectAll() {
  if (mCurrentEditor != nullptr)
    mCurrentEditor->selectAllPoints();
}

void MainWindow::toolChanged(QAction *toolAction) {
  bool selection = false;
  SelectionMode prevMode = mCurrentMode;
  Qt::CursorShape cursor = Qt::ArrowCursor;

  if (toolAction == mActionSelectionTool) {
    mCurrentMode = SelectionMode::Selection;
    selection = true;
  }
  else if (toolAction == mActionHandTool) {
    mCurrentMode = SelectionMode::Hand;
  }
  else if (toolAction == mActionAddTool) {
    mCurrentMode = SelectionMode::AddPoints;
    cursor = Qt::CrossCursor;
  }

  ui->planSet->setCursor(cursor);
  mSelectionCollapsible->setVisible(selection);

  if (prevMode != mCurrentMode && mCurrentEditor != nullptr)
    mCurrentEditor->setSelectionMode(mCurrentMode);
}

void MainWindow::findProblems() {
  // TODO Create the PlanErrorChecker subclass that creates a QString with the
  // TODO problems found and put that string inside a dialog
}

void MainWindow::invertPoints() {
  if (mCurrentEditor != nullptr)
    mCurrentEditor->invertPointsOrder();
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About"),
                     tr("<b>FlatMesher</b><tr/>"
                        "Version %1. Copyright Sergio M. Afonso Fumero 2015").arg(config::VERSION_STRING));
}

void MainWindow::onCursorMoved(const flat::Point2& pos) {
  mCursorPos->setText(tr("(X = %1, Y = %2)").arg(pos.getX()).arg(pos.getY()));
}

void MainWindow::onViewportChanged(const flat::Rectangle& viewport) {
  if (viewport != mViewport->viewport())
    mViewport->setViewport(viewport);
}

void MainWindow::onPointsAmountChanged(int /*diff*/) {
  mElementCounter->setText(tr("%1 elements").arg(mCurrentEditor->pointCount()));
}

void MainWindow::onSelectionChanged(SelectedItems selectionType) {
  bool pointSelected = false;
  flat::Point2 point;
  flat::Line2 line;

  QWidget *previous = mSelectionCollapsible->content();
  mSelectionCollapsible->setContent(nullptr);

  if (previous) {
    previous->setVisible(false);
    previous->setParent(this);
  }

  switch (selectionType) {
  case SelectedItems::Point:
    point = mCurrentEditor->selectedPoint();
    mPointX->setValue(point.getX());
    mPointY->setValue(point.getY());
    pointSelected = true;
    // fall-through
  case SelectedItems::PointSet:
    mSelectionCollapsible->setContent(mSelectionPoint);
    mPointX->setEnabled(pointSelected);
    mPointY->setEnabled(pointSelected);
    mPointMove->setEnabled(pointSelected);
    break;
  case SelectedItems::Line:
    line = mCurrentEditor->selectedLine();
    mLineLength->setText(tr("Length: %1").arg(line.length()));
    mLineSlope->setText(tr("Slope: %1").arg(line.slope()));

    mSelectionCollapsible->setContent(mSelectionLine);
    break;
  }
}

int MainWindow::findOpenFile(const QString& fileName) const {
  for (int i = 0; i < ui->planSet->count(); ++i) {
    MeshEditor *editor = dynamic_cast<MeshEditor*>(ui->planSet->widget(i));
    if (editor->fileName().compare(fileName) == 0)
      return i;
  }

  return -1;
}

void MainWindow::configureAndSelectEditor(MeshEditor* editor,
                                          const QString& tabName) {
  editor->layout()->setContentsMargins(0 ,0, 0, 0);
  editor->setGridVisible(ui->actionShowGrid->isChecked());
  flat::Rectangle vp = editor->viewport();

  int index = ui->planSet->addTab(editor, tabName);
  ui->planSet->setCurrentIndex(index);

  editor->setViewport(vp);
  mViewport->setViewport(editor->viewport());
}

void MainWindow::onTabChanged(int tabIndex) {
  // Disconnect every signal between the main window and the previous active
  // editor
  if (mCurrentEditor != nullptr) {
    disconnect(this, nullptr, mCurrentEditor, nullptr);
    disconnect(mCurrentEditor, nullptr, this, nullptr);
  }

  if (tabIndex >= 0) {
    mCurrentEditor = dynamic_cast<MeshEditor*>(ui->planSet->widget(tabIndex));
    mCurrentEditor->setSelectionMode(mCurrentMode);

    mUndoGroup->setActiveStack(mCurrentEditor->undoStack());
    mTriangleSz->setValue(mCurrentEditor->triangleSize());
    mWallsHeight->setValue(mCurrentEditor->wallsHeight());
    mViewport->setViewport(mCurrentEditor->viewport());

    onSelectionChanged(mCurrentEditor->selectionType());

    connect(mCurrentEditor, SIGNAL(cursorMoved(flat::Point2)), this, SLOT(onCursorMoved(flat::Point2)));
    connect(mCurrentEditor, SIGNAL(viewportChanged(flat::Rectangle)), this, SLOT(onViewportChanged(flat::Rectangle)));
    connect(mCurrentEditor, SIGNAL(pointsAmountChanged(int)), this, SLOT(onPointsAmountChanged(int)));
    connect(mCurrentEditor, SIGNAL(selectionChanged(SelectedItems)), this, SLOT(onSelectionChanged(SelectedItems)));

    ui->actionShowGrid->setChecked(mCurrentEditor->isGridVisible());
    editorAvailable(true);
  }
  else {
    mCurrentEditor = nullptr;
    mUndoGroup->setActiveStack(nullptr);
    onSelectionChanged(SelectedItems::None);

    editorAvailable(false);
    resetInputsToDefault();
  }
}

void MainWindow::onTabClose(int tabIndex) {
  onTabChanged(tabIndex);
  closeFlat();
}

void MainWindow::onGridVisibilityChanged(bool visible) {
  if (mCurrentEditor != nullptr)
    mCurrentEditor->setGridVisible(visible);
}

void MainWindow::onGeneralApplyClicked() {
  if (mCurrentEditor != nullptr) {
    mCurrentEditor->setTrianglesSize(mTriangleSz->value());
    mCurrentEditor->setWallsHeight(mWallsHeight->value());
  }

  mViewport->setStepSize(mTriangleSz->value());
  mPointX->setSingleStep(mTriangleSz->value());
  mPointY->setSingleStep(mTriangleSz->value());
}

void MainWindow::onViewportApplyClicked() {
  if (mCurrentEditor != nullptr)
    mCurrentEditor->setViewport(mViewport->viewport());
}

void MainWindow::onViewportResetClicked() {
  if (mCurrentEditor != nullptr)
    mCurrentEditor->adjustViewport();
}

void MainWindow::onPointMoveClicked() {
  if (mCurrentEditor != nullptr && mCurrentEditor->selectionType() == SelectedItems::Point)
    mCurrentEditor->changeSelectedPoint(flat::Point2(mPointX->value(), mPointY->value()));
}

void MainWindow::onPointDeleteClicked() {
  if (mCurrentEditor != nullptr) {
    SelectedItems selType = mCurrentEditor->selectionType();
    if (selType == SelectedItems::Point || selType == SelectedItems::PointSet)
      mCurrentEditor->deleteSelectedPoints();
  }
}

void MainWindow::onLineSplitClicked() {
  if (mCurrentEditor != nullptr && mCurrentEditor->selectionType() == SelectedItems::Line)
    mCurrentEditor->splitSelectedLine();
}

bool MainWindow::saveEditorModel(int tabIndex) {
  if (tabIndex >= 0 && tabIndex < ui->planSet->count()) {
    MeshEditor *editor = dynamic_cast<MeshEditor*>(ui->planSet->widget(tabIndex));

    if (editor && !editor->isSaved()) {
      flat::FloorPlan plan = editor->plan();

      QString fileName;
      if (!editor->fileName().isNull()) {
        fileName = editor->fileName();
        FileManager::saveFlat(plan, fileName);
      }
      else
        fileName = FileManager::saveFlat(plan);

      if (!fileName.isNull()) {
        QString file = fileName.section('/', -1);
        ui->planSet->tabBar()->setTabText(tabIndex, file);
      }
    }

    // TODO Check the actual error code from the filemanager
    return true;
  }

  return false;
}

void MainWindow::resetInputsToDefault() {
  mTriangleSz->setRange(config::MIN_TRIANGLE_SZ, config::MAX_TRIANGLE_SZ);
  mTriangleSz->setSingleStep(config::DEFAULT_TRIANGLE_SZ);
  mTriangleSz->setValue(config::DEFAULT_TRIANGLE_SZ);

  mWallsHeight->setRange(config::MIN_WALLS_HEIGHT, config::MAX_WALLS_HEIGHT);
  mWallsHeight->setSingleStep(config::DELTA_WALLS_HEIGHT);
  mWallsHeight->setValue(config::DEFAULT_WALLS_HEIGHT);

  mPointX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mPointX->setSingleStep(config::DEFAULT_TRIANGLE_SZ);
  mPointX->setValue(0.0);

  mPointY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mPointY->setSingleStep(config::DEFAULT_TRIANGLE_SZ);
  mPointY->setValue(0.0);

  mViewport->resetInputsToDefault();
}

void MainWindow::editorAvailable(bool available) {
  ui->actionClose->setEnabled(available);
  ui->actionCloseAll->setEnabled(available);
  ui->actionSave->setEnabled(available);
  ui->actionSaveAs->setEnabled(available);
  ui->actionSaveAll->setEnabled(available);
  ui->actionExport->setEnabled(available);
  ui->actionSelectAll->setEnabled(available);
  ui->actionFindProblems->setEnabled(available);
  ui->actionInvertPoints->setEnabled(available);
}

void MainWindow::setupActions() {
  mToolActions = new QActionGroup(this);
  mActionSelectionTool = new QAction(QPixmap(":/img/cursor-arrow.png"), tr("Selection tool"), this);
  mActionHandTool = new QAction(QPixmap(":/img/cursor-openhand.png"), tr("Hand tool"), this);
  mActionAddTool = new QAction(QPixmap(":/img/cursor-cross.png"), tr("Points addition tool"), this);

  mActionSelectionTool->setCheckable(true);
  mActionSelectionTool->setChecked(true);
  mActionHandTool->setCheckable(true);
  mActionAddTool->setCheckable(true);

  mActionSelectionTool->setActionGroup(mToolActions);
  mActionHandTool->setActionGroup(mToolActions);
  mActionAddTool->setActionGroup(mToolActions);

  QList<QAction*> actGroup = mToolActions->actions();
  ui->menuTools->insertActions(ui->actionFindProblems, actGroup);
  ui->menuTools->insertSeparator(ui->actionFindProblems);
  ui->toolBar->addActions(actGroup);

  mUndoGroup = new QUndoGroup(this);
  QAction *undoAction = mUndoGroup->createUndoAction(this);
  QAction *redoAction = mUndoGroup->createRedoAction(this);

  undoAction->setIcon(QPixmap(":/img/undo.png"));
  redoAction->setIcon(QPixmap(":/img/redo.png"));

  ui->menuEdit->insertAction(ui->actionSelectAll, undoAction);
  ui->menuEdit->insertAction(ui->actionSelectAll, redoAction);
  ui->menuEdit->insertSeparator(ui->actionSelectAll);

  ui->toolBar->addSeparator();
  ui->toolBar->addAction(undoAction);
  ui->toolBar->addAction(redoAction);

  ui->actionNew->setShortcut(QKeySequence::New);
  ui->actionOpen->setShortcut(QKeySequence::Open);
  ui->actionClose->setShortcut(QKeySequence::Close);
  ui->actionSave->setShortcut(QKeySequence::Save);
  ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
  ui->actionExit->setShortcut(QKeySequence::Quit);

  undoAction->setShortcut(QKeySequence::Undo);
  redoAction->setShortcut(QKeySequence::Redo);
  ui->actionSelectAll->setShortcut(QKeySequence::SelectAll);

  mActionSelectionTool->setShortcut(Qt::Key_S);
  mActionHandTool->setShortcut(Qt::Key_D);
  mActionAddTool->setShortcut(Qt::Key_F);

  connect(ui->actionNew,            SIGNAL(triggered()),             this, SLOT(newFlat()));
  connect(ui->actionOpen,           SIGNAL(triggered()),             this, SLOT(openFlat()));
  connect(ui->actionClose,          SIGNAL(triggered()),             this, SLOT(closeFlat()));
  connect(ui->actionCloseAll,       SIGNAL(triggered()),             this, SLOT(closeAllFlats()));
  connect(ui->actionSave,           SIGNAL(triggered()),             this, SLOT(saveFlat()));
  connect(ui->actionSaveAll,        SIGNAL(triggered()),             this, SLOT(saveAllFlats()));
  connect(ui->actionSaveAs,         SIGNAL(triggered()),             this, SLOT(saveFlatAs()));
  connect(ui->actionExport,         SIGNAL(triggered()),             this, SLOT(exportMesh()));
  connect(ui->actionExit,           SIGNAL(triggered()),             this, SLOT(close()));

  connect(ui->actionSelectAll,      SIGNAL(triggered()),             this, SLOT(selectAll()));

  connect(ui->actionFindProblems,   SIGNAL(triggered()),             this, SLOT(findProblems()));
  connect(ui->actionInvertPoints,   SIGNAL(triggered()),             this, SLOT(invertPoints()));

  connect(ui->actionShowGrid,       SIGNAL(toggled(bool)),           this, SLOT(onGridVisibilityChanged(bool)));
  connect(ui->actionAbout,          SIGNAL(triggered()),             this, SLOT(about()));
  connect(ui->planSet,              SIGNAL(currentChanged(int)),     this, SLOT(onTabChanged(int)));
  connect(ui->planSet,              SIGNAL(tabCloseRequested(int)),  this, SLOT(onTabClose(int)));
  connect(ui->actionShowProperties, SIGNAL(toggled(bool)),           ui->dockWidget, SLOT(setVisible(bool)));
  connect(ui->dockWidget,           SIGNAL(visibilityChanged(bool)), ui->actionShowProperties, SLOT(setChecked(bool)));
  connect(mToolActions,             SIGNAL(triggered(QAction*)),     this, SLOT(toolChanged(QAction*)));
}

void MainWindow::setupPropertiesSidebar() {
  // General options
  QWidget *general = new QWidget(this);

  mTriangleSz = new QDoubleSpinBox(general);
  mWallsHeight = new QDoubleSpinBox(general);
  QLabel *triangleLabel = new QLabel(tr("Triangle size"), general);
  QLabel *wallsLabel = new QLabel(tr("Walls height"), general);
  QPushButton *generalApply = new QPushButton(tr("Apply"), general);

  QGridLayout *layout = new QGridLayout(general);
  layout->setMargin(0);
  layout->addWidget(triangleLabel, 0, 0, 1, 1);
  layout->addWidget(wallsLabel,    1, 0, 1, 1);
  layout->addWidget(mTriangleSz,   0, 1, 1, 1);
  layout->addWidget(mWallsHeight,  1, 1, 1, 1);
  layout->addWidget(generalApply,  2, 0, 1, 1);

  CollapsibleWidget *generalCollapsible = new CollapsibleWidget(tr("Properties"), general, this);
  ui->propertiesWidget->layout()->addWidget(generalCollapsible);

  connect(generalApply, SIGNAL(clicked()), this, SLOT(onGeneralApplyClicked()));

  // Viewport options
  QWidget *viewport = new QWidget(this);

  mViewport = new ViewportControls(viewport);
  QPushButton *viewportReset = new QPushButton(tr("Reset"), viewport);
  QPushButton *viewportApply = new QPushButton(tr("Apply"), viewport);

  layout = new QGridLayout(viewport);
  layout->setMargin(0);
  layout->addWidget(mViewport,     0, 0, 2, 4);
  layout->addWidget(viewportApply, 2, 0, 1, 2);
  layout->addWidget(viewportReset, 2, 2, 1, 2);

  CollapsibleWidget *viewportCollapsible = new CollapsibleWidget(tr("Viewport"), viewport, this);
  ui->propertiesWidget->layout()->addWidget(viewportCollapsible);

  connect(viewportApply, SIGNAL(clicked()), this, SLOT(onViewportApplyClicked()));
  connect(viewportReset, SIGNAL(clicked()), this, SLOT(onViewportResetClicked()));

  // Selection options (Shared)
  mSelectionCollapsible = new CollapsibleWidget(tr("Selection"), nullptr, this);
  ui->propertiesWidget->layout()->addWidget(mSelectionCollapsible);

  // Selection options (Points)
  mSelectionPoint = new QWidget(this);
  mSelectionPoint->setVisible(false);

  mPointX = new QDoubleSpinBox(mSelectionPoint);
  mPointY = new QDoubleSpinBox(mSelectionPoint);
  mPointMove = new QPushButton(tr("Apply"), mSelectionPoint);
  QLabel *xLabel = new QLabel(tr("X"), mSelectionPoint);
  QLabel *yLabel = new QLabel(tr("Y"), mSelectionPoint);
  QPushButton *selectionDelete = new QPushButton(tr("Delete point/s"), mSelectionPoint);

  layout = new QGridLayout(mSelectionPoint);
  layout->setMargin(0);
  layout->addWidget(xLabel,          0, 0, 1, 1);
  layout->addWidget(mPointX,         0, 1, 1, 1);
  layout->addWidget(yLabel,          0, 2, 1, 1);
  layout->addWidget(mPointY,         0, 3, 1, 1);
  layout->addWidget(mPointMove,      1, 0, 1, 2);
  layout->addWidget(selectionDelete, 1, 2, 1, 2);

  connect(mPointMove, SIGNAL(clicked()), this, SLOT(onPointMoveClicked()));
  connect(selectionDelete, SIGNAL(clicked()), this, SLOT(onPointDeleteClicked()));

  // Selection options (Lines)
  mSelectionLine = new QWidget(this);
  mSelectionLine->setVisible(false);

  mLineLength = new QLabel(tr("Length: %1").arg(0.0), mSelectionLine);
  mLineSlope = new QLabel(tr("Slope: %1").arg(0.0), mSelectionLine);
  QPushButton *selectionSplitLine = new QPushButton(tr("Split line"), mSelectionLine);

  layout = new QGridLayout(mSelectionLine);
  layout->setMargin(0);
  layout->addWidget(mLineLength,        0, 0, 1, 1);
  layout->addWidget(mLineSlope,         0, 1, 1, 1);
  layout->addWidget(selectionSplitLine, 1, 0, 1, 1);

  connect(selectionSplitLine, SIGNAL(clicked()), this, SLOT(onLineSplitClicked()));

  // Make all the widgets the same size
  QSize minSize = general->sizeHint();
  minSize = minSize.expandedTo(viewport->sizeHint());
  minSize = minSize.expandedTo(mSelectionPoint->sizeHint());
  minSize = minSize.expandedTo(mSelectionLine->sizeHint());

  general->setMinimumSize(minSize);
  viewport->setMinimumSize(minSize);
  mSelectionPoint->setMinimumSize(minSize);
  mSelectionLine->setMinimumSize(minSize);
  ui->dockWidget->setMinimumSize(generalCollapsible->sizeHint() + QSize(5, 5));

  resetInputsToDefault();
}

void MainWindow::setupStatusBar() {
  mCursorPos = new QLabel("(X = 0.00, Y = 0.00)", this);
  mElementCounter = new QLabel(tr("%1 elements").arg(0), this);

  ui->statusBar->addWidget(mCursorPos, 1);
  ui->statusBar->addWidget(mElementCounter);
}
