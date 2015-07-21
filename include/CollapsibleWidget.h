#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QWidget>

class QLabel;
class QVBoxLayout;
class QToolButton;

class CollapsibleWidget: public QWidget {
  Q_OBJECT

public:
  CollapsibleWidget(QString title, QWidget *content, QWidget *parent = 0);
  virtual ~CollapsibleWidget();

public slots:
  void setTitle(QString title);
  void setContent(QWidget* content);

private slots:
  void toggle();

private:
  bool mVisible;

  QLabel *mTitle;
  QWidget *mContent;
  QToolButton *mCollapse;
  QVBoxLayout *mLayout;

};

#endif // COLLAPSIBLEWIDGET_H
