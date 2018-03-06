#ifndef MESHANALYZER_H
#define MESHANALYZER_H

#include <QDialog>

namespace Ui {
class MeshAnalyzer;
}

namespace flat {
class FloorPlan;
}

class MeshAnalyzer: public QDialog {
  Q_OBJECT

public:
  explicit MeshAnalyzer(QWidget *parent = 0);
  ~MeshAnalyzer();

  void processMesh(const flat::FloorPlan& plan);

public slots:
  void setProcessingCompleted(int percentage);
  void addMessage(const QString& message, const QColor& color);

private:
  Ui::MeshAnalyzer *ui;

};

#endif // MESHANALYZER_H
