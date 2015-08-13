#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <QDoubleSpinBox>

class CustomSpinbox: public QDoubleSpinBox {
  Q_OBJECT

public:
  CustomSpinbox(QWidget *parent = 0);

};

#endif // CUSTOMSPINBOX_H
