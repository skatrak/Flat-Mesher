#include "CollapsibleWidget.h"

#include "Configuration.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

CollapsibleWidget::CollapsibleWidget(QString title, QWidget* content,
                                     QWidget* parent):
  QWidget(parent), mVisible(true), mTitle(0), mContent(0) {

  setTitle(title);

  mCollapse = new QToolButton(this);
  mCollapse->setArrowType(Qt::DownArrow);
  mCollapse->setAutoRaise(true);

  mLayout = new QVBoxLayout(this);

  QHBoxLayout *titleLayout = new QHBoxLayout;
  titleLayout->addWidget(mTitle, 1);
  titleLayout->addWidget(mCollapse);

  mLayout->addLayout(titleLayout);
  setContent(content);

  connect(mCollapse, SIGNAL(clicked()), this, SLOT(toggle()));
}

CollapsibleWidget::~CollapsibleWidget() {

}

void CollapsibleWidget::setTitle(QString title) {
  if (mTitle)
    mTitle->setText(title);
  else {
    mTitle = new QLabel(title, this);

    QFont font = mTitle->font();
    font.setBold(true);
    font.setPointSize(config::HEADER_FONT_SZ);
    mTitle->setFont(font);
  }
}

void CollapsibleWidget::setContent(QWidget* content) {
  if (mContent) {
    mLayout->removeWidget(mContent);
    delete mContent;
  }

  if (content) {
    mLayout->addWidget(content, 1);
    content->setVisible(mVisible);
  }

  mContent = content;
}

void CollapsibleWidget::toggle() {
  mVisible = !mVisible;

  if (mContent)
    mContent->setVisible(mVisible);

  if (mVisible)
    mCollapse->setArrowType(Qt::DownArrow);
  else
    mCollapse->setArrowType(Qt::RightArrow);
}
