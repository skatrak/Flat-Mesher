#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QWidget>

class QLabel;
class QGridLayout;
class QToolButton;

class CollapsibleWidget: public QWidget {
  Q_OBJECT

public:
  CollapsibleWidget(QString title, QWidget *content, QWidget *parent = 0);
  QWidget* content() const { return mContent; }

public slots:
  void setTitle(QString title);
  void setContent(QWidget* content);

signals:
  void collapseChanged(bool collapsed);

private slots:
  void toggle();

private:
  bool mVisible;

  QLabel *mTitle;
  QWidget *mContent;
  QToolButton *mCollapse;
  QGridLayout *mLayout;

};

#endif // COLLAPSIBLEWIDGET_H
