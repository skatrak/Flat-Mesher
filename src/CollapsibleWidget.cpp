#include "CollapsibleWidget.h"

#include "Configuration.h"

#include <QGridLayout>
#include <QToolButton>
#include <QLabel>

CollapsibleWidget::CollapsibleWidget(QString title, QWidget* content,
                                     QWidget* parent):
  QWidget(parent), mVisible(true), mTitle(0), mContent(0) {

  setTitle(title);

  mCollapse = new QToolButton(this);
  mCollapse->setArrowType(Qt::DownArrow);
  mCollapse->setAutoRaise(true);

  mLayout = new QGridLayout(this);

  mLayout->addWidget(mTitle, 0, 0, 1, 1);
  mLayout->addWidget(mCollapse, 0, 1, 1, 1, Qt::AlignRight);

  setContent(content);

  connect(mCollapse, SIGNAL(clicked()), this, SLOT(toggle()));
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
  if (content != mContent) {
    if (mContent)
      mLayout->removeWidget(mContent);

    if (content) {
      mLayout->addWidget(content, 1, 0, 1, 2, Qt::AlignLeft);
      content->setVisible(mVisible);
    }

    mContent = content;
  }
}

void CollapsibleWidget::toggle() {
  mVisible = !mVisible;

  if (mContent)
    mContent->setVisible(mVisible);

  if (mVisible)
    mCollapse->setArrowType(Qt::DownArrow);
  else
    mCollapse->setArrowType(Qt::RightArrow);

  emit collapseChanged(!mVisible);
}
