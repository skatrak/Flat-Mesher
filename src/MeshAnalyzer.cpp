#include "MeshAnalyzer.h"
#include "ui_MeshAnalyzer.h"

#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/Line2.h>
#include <FlatMesher/PlanErrorChecker.h>
#include <FlatMesher/Point2.h>

// This class is defined here because it's private
class ErrorChecker: public flat::PlanErrorChecker {
public:
  ErrorChecker(MeshAnalyzer *parent):
      mParent(parent), mErrorsFound(false), mProgressColor(Qt::darkYellow),
      mErrorColor(Qt::red) {}

  virtual void visitCheckBasicProperties() {
    mParent->addMessage(MeshAnalyzer::tr("Checking basic properties..."), mProgressColor);
    mParent->setProcessingCompleted(0);
  }

  virtual void visitCheckSegmentsProperties() {
    mParent->addMessage(MeshAnalyzer::tr("Checking segments properties..."), mProgressColor);
    mParent->setProcessingCompleted(20);
  }

  virtual void visitCheckPointsOrder() {
    mParent->addMessage(MeshAnalyzer::tr("Checking points order..."), mProgressColor);
    mParent->setProcessingCompleted(40);
  }

  virtual void visitCheckRepeatedPoints() {
    mParent->addMessage(MeshAnalyzer::tr("Searching for repeated points..."), mProgressColor);
    mParent->setProcessingCompleted(60);
  }

  virtual void visitCheckSegmentsIntersections() {
    mParent->addMessage(MeshAnalyzer::tr("Checking segments intersections..."), mProgressColor);
    mParent->setProcessingCompleted(80);
  }

  virtual bool visitInsufficientNodes(size_t nodes) {
    mParent->addMessage(MeshAnalyzer::tr("There are not enough points: %1.").arg(nodes), mErrorColor);
    mErrorsFound = true;
    return true;
  }

  virtual bool visitInvalidTriangleSize(double triangle_size) {
    mParent->addMessage(MeshAnalyzer::tr("The triangle size is not valid: %1.").arg(triangle_size), mErrorColor);
    mErrorsFound = true;
    return true;
  }

  virtual bool visitInvalidHeight(double height) {
    mParent->addMessage(MeshAnalyzer::tr("The height specified is not valid: %1.").arg(height), mErrorColor);
    mErrorsFound = true;
    return true;
  }

  virtual bool visitInvalidSegmentLength(const flat::Line2& segment) {
    mParent->addMessage(MeshAnalyzer::tr("The segment %1 doesn't have a valid length: %2.").arg(print(segment)).arg(segment.length()), mErrorColor);
    mErrorsFound = true;
    return false;
  }

  virtual bool visitInvalidSegmentSlope(const flat::Line2& segment) {
    mParent->addMessage(MeshAnalyzer::tr("The segment %1 doesn't have a valid slope: %2.").arg(print(segment)).arg(segment.slope()), mErrorColor);
    mErrorsFound = true;
    return false;
  }

  virtual bool visitNotCCWOrder() {
    mParent->addMessage(MeshAnalyzer::tr("Points are not in counter-clockwise order."), mErrorColor);
    mErrorsFound = true;
    return false;
  }

  virtual bool visitRepeatedPoint(const flat::Point2& point) {
    mParent->addMessage(MeshAnalyzer::tr("The point %1 is repeated.").arg(print(point)), mErrorColor);
    mErrorsFound = true;
    return false;
  }

  virtual bool visitIntersectingSegments(const flat::Line2& first,
                                         const flat::Line2& second) {
    mParent->addMessage(MeshAnalyzer::tr("The segments %1 and %2 intersect.").arg(print(first)).arg(print(second)), mErrorColor);
    mErrorsFound = true;
    return false;
  }

  bool errorsFound() const {
    return mErrorsFound;
  }

private:
  QString print(const flat::Point2& point) {
    return QString("(%1, %2)").arg(point.getX()).arg(point.getY());
  }

  QString print(const flat::Line2& line) {
    return QString("[%1 -> %2]").arg(print(line.getA())).arg(print(line.getB()));
  }

  MeshAnalyzer *mParent;
  bool mErrorsFound;
  QColor mProgressColor, mErrorColor;

};

// ////////////////////////////////////////////
// MeshAnalyzer implementation starts here
// ////////////////////////////////////////////

MeshAnalyzer::MeshAnalyzer(QWidget *parent):
    QDialog(parent), ui(new Ui::MeshAnalyzer) {
  ui->setupUi(this);
}

MeshAnalyzer::~MeshAnalyzer() {
  delete ui;
}

void MeshAnalyzer::processMesh(const flat::FloorPlan& plan) {
  ErrorChecker checker(this);

  if (!plan.checkErrors(&checker))
    addMessage(tr("Error checking aborted. Solve the problems highlighted and try again."), QColor(Qt::darkRed));
  else
    setProcessingCompleted(100);

  if (!checker.errorsFound()) {
    addMessage(tr("No errors found."), QColor(Qt::green));

    flat::FlatMesh mesh;
    mesh.createFromPlan(&plan);

    ui->labelPoints->setText(tr("<b>Elements:</b> %1").arg(mesh.getNodes().size()));
    ui->labelTriangles->setText(tr("<b>Triangles:</b> %1").arg(mesh.getMesh().size()));
  }
}

void MeshAnalyzer::setProcessingCompleted(int percentage) {
  ui->progressBar->setValue(percentage);
}

void MeshAnalyzer::addMessage(const QString& message, const QColor& color) {
  ui->text->append(message);

  QTextCursor cur(ui->text->document());
  cur.movePosition(QTextCursor::End);

  QTextCharFormat fmt;
  fmt.setForeground(QBrush(color));

  cur.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, message.length());
  cur.setCharFormat(fmt);
}
