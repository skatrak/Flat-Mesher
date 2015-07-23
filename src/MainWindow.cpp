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

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->actionClose->setEnabled(false);

  mUndoGroup = new QUndoGroup(this);
  ui->menuEdit->addSeparator();
  ui->menuEdit->addAction(mUndoGroup->createUndoAction(this));
  ui->menuEdit->addAction(mUndoGroup->createRedoAction(this));

  createToolbarActions();
  createPropertiesSidebar();
  createStatusBar();

  connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFlat()));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFlat()));
  connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeFlat()));
  connect(ui->actionCloseAll, SIGNAL(triggered()), this, SLOT(closeAllFlats()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFlat()));
  connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFlatAs()));
  connect(ui->actionSaveAll, SIGNAL(triggered()), this, SLOT(saveAllFlats()));
  connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(exportMesh()));
  connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
  connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
  connect(ui->actionFindProblems, SIGNAL(triggered()), this, SLOT(findProblems()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->planSet, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

  connect(mToolActions, SIGNAL(triggered(QAction*)), this, SLOT(toolChanged(QAction*)));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::newFlat() {
  ui->planSet->addTab(new MeshEditor(this), tr("<unnamed>"));
  ui->planSet->setCurrentIndex(ui->planSet->count() - 1);

  ui->actionClose->setEnabled(true);
}

void MainWindow::openFlat() {
  flat::FloorPlan plan;
  QString fileName = FileManager::openFlat(plan);

  if (!fileName.isNull()) {
    QString file = fileName.section('/', -1);
    // TODO If it exists, only swap the current tab (ask if user wants to replace)
    ui->planSet->addTab(new MeshEditor(fileName, plan, this), file);
    ui->planSet->setCurrentIndex(ui->planSet->count() - 1);

    ui->actionClose->setEnabled(true);
  }
}

void MainWindow::closeFlat() {
  if (ui->planSet->count() > 0) {
    // TODO Ask first, and only if an editor is selected
    if (!mCurrentEditor->undoStack()->isClean())
      FileManager::saveFlat(mCurrentEditor->plan(), mCurrentEditor->fileName());

    ui->planSet->removeTab(ui->planSet->currentIndex());

    if (ui->planSet->count() == 0)
      ui->actionClose->setEnabled(false);
  }
}

void MainWindow::closeAllFlats() {
  while (ui->planSet->count() > 0) {
    // TODO Save if it has been modified
    ui->planSet->removeTab(0);
  }

  ui->actionClose->setEnabled(false);
}

void MainWindow::saveFlat() {

}

void MainWindow::saveFlatAs() {
  // TODO Only when an editor is selected
  // TODO Extract the FloorPlan from the current graphics widget
  flat::FloorPlan plan = mCurrentEditor->plan();

  // TODO In saveFlat() it has to be checked whether it is representing an
  // TODO existing file or not
  QString fileName = FileManager::saveFlat(plan);
  if (!fileName.isNull()) {
    QString file = fileName.section('/', -1);
    ui->planSet->tabBar()->setTabText(ui->planSet->currentIndex(), file);
  }
}

void MainWindow::saveAllFlats() {

}

void MainWindow::exportMesh() {
  // TODO Only when an editor is selected
  FileManager::saveMesh(mCurrentEditor->mesh());
}

// TODO Change for the provided ones
void MainWindow::undo() {

}

void MainWindow::redo() {

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

  if (prevMode != mCurrentMode)
    emit changeMode(mCurrentMode);
}

void MainWindow::findProblems() {
  // TODO Extend the library to test everything and return all problems found
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About"),
                     tr("<b>FlatMesher</b><tr/>"
                        "Version %1. Copyright Sergio M. Afonso Fumero 2015").arg(config::VERSION_STRING));
}

void MainWindow::generalApplyClicked() {
  emit changeGeneralProperties(mTriangleSz->value(), mWallsHeight->value());
}

void MainWindow::viewportApplyClicked() {
  double minX = mViewport->minX(), maxX = mViewport->maxX();
  double minY = mViewport->minY(), maxY = mViewport->maxY();

  emit changeViewPort(flat::Rectangle(maxY, minY, minX, maxX));
}

void MainWindow::selectionApplyClicked() {
  emit moveSelectedPoint(flat::Point2(mPointX->value(), mPointY->value()));
}

void MainWindow::tabChanged(int tabIndex) {
  if (tabIndex >= 0) {
    mCurrentEditor = dynamic_cast<MeshEditor*>(ui->planSet->widget(tabIndex));
    mUndoGroup->setActiveStack(mCurrentEditor->undoStack());
    // TODO Update actions?
  }
  else {
    mCurrentEditor = nullptr;
    mUndoGroup->setActiveStack(nullptr);
  }
}

void MainWindow::createToolbarActions() {
  mToolActions = new QActionGroup(this);
  mActionSelectionTool = new QAction(QPixmap(":/img/cursor-arrow.png"), tr("Selection tool"), this);
  mActionHandTool = new QAction(QPixmap(":/img/cursor-openhand.png"), tr("Hand tool"), this);
  mActionAddTool = new QAction(QPixmap(":/img/cursor-cross.png"), tr("Points addition tool"), this);
  mActionFindProblems = new QAction(tr("Find problems..."), this);

  mActionSelectionTool->setShortcut(Qt::Key_S);
  mActionHandTool->setShortcut(Qt::Key_D);
  mActionAddTool->setShortcut(Qt::Key_F);

  mActionSelectionTool->setCheckable(true);
  mActionSelectionTool->setChecked(true);
  mActionHandTool->setCheckable(true);
  mActionAddTool->setCheckable(true);

  mActionSelectionTool->setActionGroup(mToolActions);
  mActionHandTool->setActionGroup(mToolActions);
  mActionAddTool->setActionGroup(mToolActions);

  QList<QAction*> actGroup = mToolActions->actions();
  ui->menuTools->addActions(actGroup);
  ui->toolBar->addActions(actGroup);

  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionFindProblems);
}

void MainWindow::createPropertiesSidebar() {
  // General options
  QWidget *general = new QWidget(this);

  QGridLayout *layout = new QGridLayout(general);
  layout->setMargin(0);

  layout->addWidget(new QLabel(tr("Triangle size"), general), 0, 0, 1, 1);
  layout->addWidget(new QLabel(tr("Walls height"), general), 1, 0, 1, 1);

  mTriangleSz = new QDoubleSpinBox(general);
  mTriangleSz->setRange(config::MIN_TRIANGLE_SZ, config::MAX_TRIANGLE_SZ);
  mTriangleSz->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mTriangleSz->setValue(config::DEFAULT_TRIANGLE_SZ);
  layout->addWidget(mTriangleSz, 0, 1, 1, 1);

  mWallsHeight = new QDoubleSpinBox(general);
  mWallsHeight->setRange(config::MIN_WALLS_HEIGHT, config::MAX_WALLS_HEIGHT);
  mWallsHeight->setSingleStep(config::DELTA_WALLS_HEIGHT);
  mWallsHeight->setValue(config::DEFAULT_WALLS_HEIGHT);
  layout->addWidget(mWallsHeight, 1, 1, 1, 1);

  QPushButton *generalApply = new QPushButton(tr("Apply"), general);
  layout->addWidget(generalApply, 2, 1, 1, 1);

  // Connect generalApply
  connect(generalApply, SIGNAL(clicked()), this, SLOT(generalApplyClicked()));

  ui->propertiesWidget->layout()->addWidget(new CollapsibleWidget(tr("Properties"), general, this));

  // Viewport options
  QWidget *viewport = new QWidget(this);

  layout = new QGridLayout(viewport);
  layout->setMargin(0);

  mViewport = new ViewportControls(viewport);
  layout->addWidget(mViewport, 0, 0, 2, 4);

  QPushButton *viewportReset = new QPushButton(tr("Reset"), viewport);
  layout->addWidget(viewportReset, 2, 0, 1, 2);

  QPushButton *viewportApply = new QPushButton(tr("Apply"), viewport);
  layout->addWidget(viewportApply, 2, 2, 1, 2);

  // Connect viewportReset & viewportApply
  connect(viewportReset, SIGNAL(clicked()), this, SIGNAL(resetViewPort()));
  connect(viewportApply, SIGNAL(clicked()), this, SLOT(viewportApplyClicked()));

  ui->propertiesWidget->layout()->addWidget(new CollapsibleWidget(tr("Viewport"), viewport, this));

  // Selection options (Shared)
  mSelectionCollapsible = new CollapsibleWidget(tr("Selection"), 0, this);
  ui->propertiesWidget->layout()->addWidget(mSelectionCollapsible);

  // Selection options (Points)
  mSelectionPoint = new QWidget(this);
  mSelectionPoint->setVisible(false);

  layout = new QGridLayout(mSelectionPoint);
  layout->setMargin(0);

  layout->addWidget(new QLabel("X", mSelectionPoint), 0, 0, 1, 1);
  layout->addWidget(new QLabel("Y", mSelectionPoint), 0, 2, 1, 1);

  mPointX = new QDoubleSpinBox(mSelectionPoint);
  mPointX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mPointX->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mPointX->setValue(0.0);
  layout->addWidget(mPointX, 0, 1, 1, 1);

  mPointY = new QDoubleSpinBox(mSelectionPoint);
  mPointY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mPointY->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mPointY->setValue(0.0);
  layout->addWidget(mPointY, 0, 3, 1, 1);

  QPushButton *selectionApply = new QPushButton(tr("Apply"), mSelectionPoint);
  layout->addWidget(selectionApply, 1, 2, 1, 2);

  QPushButton *selectionDelete = new QPushButton(tr("Delete point/s"), mSelectionPoint);
  layout->addWidget(selectionDelete, 1, 0, 1, 2);

  // Connect selectionApply & selectionDelete
  connect(selectionApply, SIGNAL(clicked()), this, SLOT(selectionApplyClicked()));
  connect(selectionDelete, SIGNAL(clicked()), this, SIGNAL(deleteSelectedPoints()));

  // Selection options (Lines)
  mSelectionLine = new QWidget(this);
  mSelectionLine->setVisible(false);

  layout = new QGridLayout(mSelectionLine);
  layout->setMargin(0);

  mLineLength = new QLabel("Length: 0.00", mSelectionLine);
  mLineSlope = new QLabel("Slope: 0.00", mSelectionLine);

  layout->addWidget(mLineLength, 0, 0, 1, 1);
  layout->addWidget(mLineSlope, 0, 1, 1, 1);

  QPushButton *selectionSplitLine = new QPushButton(tr("Split line"), mSelectionLine);
  layout->addWidget(selectionSplitLine, 1, 1, 1, 1);

  // Connect selectionSplitLine
  connect(selectionSplitLine, SIGNAL(clicked()), this, SIGNAL(splitLine()));
}

void MainWindow::createStatusBar() {
  mCursorPos = new QLabel("(X = 0.00, Y = 0.00)", this);
  mElementCounter = new QLabel("0 elements", this);

  ui->statusBar->addWidget(mCursorPos, 1);
  ui->statusBar->addWidget(mElementCounter);
}
