#include "ViewportControls.h"

#include "Configuration.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

ViewportControls::ViewportControls(QWidget *parent):
    QWidget(parent), mStep(config::DELTA_TRIANGLE_SZ) {
  QGridLayout *layout = new QGridLayout(this);
  layout->setMargin(0);

  layout->addWidget(new QLabel(tr("Min. X"), this), 0, 0, 1, 1);
  layout->addWidget(new QLabel(tr("Max. X"), this), 0, 2, 1, 1);
  layout->addWidget(new QLabel(tr("Min. Y"), this), 1, 0, 1, 1);
  layout->addWidget(new QLabel(tr("Max. Y"), this), 1, 2, 1, 1);

  mMinX = new QDoubleSpinBox(this);
  mMinX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMinX->setSingleStep(mStep);
  mMinX->setValue(config::DEFAULT_VIEWPORT_MIN_X);
  layout->addWidget(mMinX, 0, 1, 1, 1);

  mMaxX = new QDoubleSpinBox(this);
  mMaxX->setRange(config::VIEWPORT_MIN_X, config::VIEWPORT_MAX_X);
  mMaxX->setSingleStep(mStep);
  mMaxX->setValue(config::DEFAULT_VIEWPORT_MAX_X);
  layout->addWidget(mMaxX, 0, 3, 1, 1);

  mMinY = new QDoubleSpinBox(this);
  mMinY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMinY->setSingleStep(mStep);
  mMinY->setValue(config::DEFAULT_VIEWPORT_MIN_Y);
  layout->addWidget(mMinY, 1, 1, 1, 1);

  mMaxY = new QDoubleSpinBox(this);
  mMaxY->setRange(config::VIEWPORT_MIN_Y, config::VIEWPORT_MAX_Y);
  mMaxY->setSingleStep(mStep);
  mMaxY->setValue(config::DEFAULT_VIEWPORT_MAX_Y);
  layout->addWidget(mMaxY, 1, 3, 1, 1);

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
