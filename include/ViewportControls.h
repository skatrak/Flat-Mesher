#ifndef VIEWPORTCONTROLS_H
#define VIEWPORTCONTROLS_H

#include <QWidget>

class QDoubleSpinBox;

class ViewportControls: public QWidget {
  Q_OBJECT

public:
  explicit ViewportControls(QWidget *parent = 0);

  double minX() const;
  double maxX() const;
  double minY() const;
  double maxY() const;

public slots:
  void setStepSize(double step);
  void setMinX(double value);
  void setMaxX(double value);
  void setMinY(double value);
  void setMaxY(double value);

private slots:
  void minXChanged(double value);
  void maxXChanged(double value);
  void minYChanged(double value);
  void maxYChanged(double value);

private:
  void adjustXRanges();
  void adjustYRanges();

  QDoubleSpinBox *mMinX, *mMaxX, *mMinY, *mMaxY;
  double mStep;

};

#endif // VIEWPORTCONTROLS_H
