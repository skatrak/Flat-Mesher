#include "ViewportControls.h"

#include "Configuration.h"

#include <FlatMesher/Rectangle.h>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

ViewportControls::ViewportControls(QWidget *parent):
    QWidget(parent), mStep(config::DEFAULT_TRIANGLE_SZ) {

  mMinX = new QDoubleSpinBox(this);
  mMaxX = new QDoubleSpinBox(this);
  mMinY = new QDoubleSpinBox(this);
  mMaxY = new QDoubleSpinBox(this);
  QLabel *minXLabel = new QLabel(tr("Min. X"), this);
  QLabel *maxXLabel = new QLabel(tr("Max. X"), this);
  QLabel *minYLabel = new QLabel(tr("Min. Y"), this);
  QLabel *maxYLabel = new QLabel(tr("Max. Y"), this);

  QGridLayout *layout = new QGridLayout(this);
  layout->setMargin(0);

  layout->addWidget(minXLabel, 0, 0, 1, 1);
  layout->addWidget(mMinX,     0, 1, 1, 1);
  layout->addWidget(maxXLabel, 0, 2, 1, 1);
  layout->addWidget(mMaxX,     0, 3, 1, 1);
  layout->addWidget(minYLabel, 1, 0, 1, 1);
  layout->addWidget(mMinY,     1, 1, 1, 1);
  layout->addWidget(maxYLabel, 1, 2, 1, 1);
  layout->addWidget(mMaxY,     1, 3, 1, 1);

  resetInputsToDefault();

  connect(mMinX, SIGNAL(valueChanged(double)), this, SLOT(minXChanged(double)));
  connect(mMaxX, SIGNAL(valueChanged(double)), this, SLOT(maxXChanged(double)));
  connect(mMinY, SIGNAL(valueChanged(double)), this, SLOT(minYChanged(double)));
  connect(mMaxY, SIGNAL(valueChanged(double)), this, SLOT(maxYChanged(double)));
}

double ViewportControls::minX() const {
  return mMinX->value();
}

double ViewportControls::maxX() const {
  return mMaxX->value();
}

double ViewportControls::minY() const {
  return mMinY->value();
}

double ViewportControls::maxY() const {
  return mMaxY->value();
}

flat::Rectangle ViewportControls::viewport() const {
  return flat::Rectangle(maxY(), minY(), minX(), maxX());
}

void ViewportControls::resetInputsToDefault() {
  mMinX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMinX->setSingleStep(mStep);
  mMinX->setValue(config::DEFAULT_VIEWPORT_MIN_X);

  mMaxX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMaxX->setSingleStep(mStep);
  mMaxX->setValue(config::DEFAULT_VIEWPORT_MAX_X);

  mMinY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMinY->setSingleStep(mStep);
  mMinY->setValue(config::DEFAULT_VIEWPORT_MIN_Y);

  mMaxY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMaxY->setSingleStep(mStep);
  mMaxY->setValue(config::DEFAULT_VIEWPORT_MAX_Y);
}

void ViewportControls::setStepSize(double step) {
  if (step > 0.0) {
    mStep = step;

    mMinX->setSingleStep(step);
    mMaxX->setSingleStep(step);
    mMinY->setSingleStep(step);
    mMaxY->setSingleStep(step);

    double diffX = mMaxX->value() - mMinX->value();
    double diffY = mMaxY->value() - mMinY->value();

    if (diffX < step)
      mMinX->setValue(mMinX->value() - step / 2.0);
    if (diffY < step)
      mMinY->setValue(mMinY->value() - step / 2.0);
  }
}

void ViewportControls::setMinX(double value) {
  mMinX->setValue(value);
}

void ViewportControls::setMaxX(double value) {
  mMaxX->setValue(value);
}

void ViewportControls::setMinY(double value) {
  mMinY->setValue(value);
}

void ViewportControls::setMaxY(double value) {
  mMaxY->setValue(value);
}

void ViewportControls::setViewport(const flat::Rectangle& viewport) {
  setMinX(viewport.getLeft());
  setMaxX(viewport.getRight());
  setMinY(viewport.getBottom());
  setMaxY(viewport.getTop());
}

void ViewportControls::minXChanged(double value) {
  mMaxX->setValue(qMax(value + mStep, mMaxX->value()));
  adjustXRanges();
}

void ViewportControls::maxXChanged(double value) {
  mMinX->setValue(qMin(value - mStep, mMinX->value()));
  adjustXRanges();
}

void ViewportControls::minYChanged(double value) {
  mMaxY->setValue(qMax(value + mStep, mMaxY->value()));
  adjustYRanges();
}

void ViewportControls::maxYChanged(double value) {
  mMinY->setValue(qMin(value - mStep, mMinY->value()));
  adjustXRanges();
}

void ViewportControls::adjustXRanges() {
  mMinX->setMaximum(mMaxX->value() - config::MIN_TRIANGLE_SZ);
  mMaxX->setMinimum(mMinX->value() + config::MIN_TRIANGLE_SZ);
}

void ViewportControls::adjustYRanges() {
  mMinY->setMaximum(mMaxY->value() - config::MIN_TRIANGLE_SZ);
  mMaxY->setMinimum(mMinY->value() + config::MIN_TRIANGLE_SZ);
}
