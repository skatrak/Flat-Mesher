#include "CustomSpinbox.h"

#include "Configuration.h"

CustomSpinbox::CustomSpinbox(QWidget *parent): QDoubleSpinBox(parent) {
  setDecimals(config::SPINBOX_DECIMALS);
  setSuffix("m");
}
