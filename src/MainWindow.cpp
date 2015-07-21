#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CollapsibleWidget.h"
#include "Configuration.h"

#include <QActionGroup>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  createToolbarActions();
  createPropertiesSidebar();
  createStatusBar();
}

MainWindow::~MainWindow() {
  delete ui;
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

  // TODO Connect generalApply

  ui->propertiesWidget->layout()->addWidget(new CollapsibleWidget(tr("Properties"), general, this));

  // Viewport options
  QWidget *viewport = new QWidget(this);

  layout = new QGridLayout(viewport);
  layout->setMargin(0);

  layout->addWidget(new QLabel(tr("Min. X"), viewport), 0, 0, 1, 1);
  layout->addWidget(new QLabel(tr("Max. X"), viewport), 0, 2, 1, 1);
  layout->addWidget(new QLabel(tr("Min. Y"), viewport), 1, 0, 1, 1);
  layout->addWidget(new QLabel(tr("Max. Y"), viewport), 1, 2, 1, 1);

  mMinX = new QDoubleSpinBox(viewport);
  mMinX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMinX->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mMinX->setValue(config::DEFAULT_VIEWPORT_MIN_X);
  layout->addWidget(mMinX, 0, 1, 1, 1);

  mMaxX = new QDoubleSpinBox(viewport);
  mMaxX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMaxX->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mMaxX->setValue(config::DEFAULT_VIEWPORT_MAX_X);
  layout->addWidget(mMaxX, 0, 3, 1, 1);

  mMinY = new QDoubleSpinBox(viewport);
  mMinY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMinY->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mMinY->setValue(config::DEFAULT_VIEWPORT_MIN_Y);
  layout->addWidget(mMinY, 1, 1, 1, 1);

  mMaxY = new QDoubleSpinBox(viewport);
  mMaxY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMaxY->setSingleStep(config::DELTA_TRIANGLE_SZ);
  mMaxY->setValue(config::DEFAULT_VIEWPORT_MAX_Y);
  layout->addWidget(mMaxY, 1, 3, 1, 1);

  QPushButton *viewportReset = new QPushButton(tr("Reset"), viewport);
  layout->addWidget(viewportReset, 2, 0, 1, 2);

  QPushButton *viewportApply = new QPushButton(tr("Apply"), viewport);
  layout->addWidget(viewportApply, 2, 2, 1, 2);

  // TODO Connect viewportReset & viewportApply

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

  QPushButton *selectionDelete = new QPushButton(tr("Delete point/s"), mSelectionPoint);
  layout->addWidget(selectionDelete, 1, 2, 1, 2);

  // TODO Connect selectionDelete

  // Selection options (Lines)
  mSelectionLine = new QWidget(this);
  mSelectionLine->setVisible(false);

  layout = new QGridLayout(mSelectionLine);
  layout->setMargin(0);

  mLineLength = new QLabel("Length: 0.00", mSelectionLine);
  mLineSlope = new QLabel("Slope: 0.00", mSelectionLine);

  layout->addWidget(mLineLength, 0, 0, 1, 1);
  layout->addWidget(mLineSlope, 0, 1, 1, 1);

  //mSelectionCollapsible->setContent(mSelectionPoint);
}

void MainWindow::createStatusBar() {
  mCursorPos = new QLabel("(X = 0.00, Y = 0.00)", this);
  mElementCounter = new QLabel("0 elements", this);

  ui->statusBar->addWidget(mCursorPos, 1);
  ui->statusBar->addWidget(mElementCounter);
}
