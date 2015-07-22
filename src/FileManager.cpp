#include "FileManager.h"

#include <fstream>
#include <string>

#include <FlatMesher/BemgenMeshFormatter.h>
#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/VTUMeshFormatter.h>
#include <QFileDialog>

QString FileManager::openFlat(flat::FloorPlan& plan) {
  QString fileName = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open flat"), QString(),
                                                  QObject::tr("FlatMesher flat files (*.flat)"));

  if (fileName.isNull() || !openFlat(fileName, plan))
    return QString();
  else
    return fileName;
}

bool FileManager::openFlat(const QString& fileName, flat::FloorPlan& plan) {
  std::ifstream input;
  input.open(fileName.toStdString());

  if (input.is_open()) {
    input >> plan;
    return input;
  }

  return false;
}

QString FileManager::saveFlat(const flat::FloorPlan &plan) {
  QString fileName = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save flat"), QString(),
                                                  QObject::tr("FlatMesher flat files(*.flat)"));

  if (fileName.isNull() || !saveFlat(plan, fileName))
    return QString();
  else
    return fileName;
}

bool FileManager::saveFlat(const flat::FloorPlan& plan, const QString& fileName) {
  std::ofstream output;
  output.open(fileName.toStdString());

  if (output.is_open()) {
    output << plan;
    return true;
  }

  return false;
}

QString FileManager::saveMesh(const flat::FlatMesh &plan) {
  QString filter;
  QString fileName = QFileDialog::getSaveFileName(nullptr, QObject::tr("Export mesh"), QString(),
                                                  QObject::tr("BEMGEN files(*.txt);;VTU files(*.vtu)"),
                                                  &filter);

  if (fileName.isNull())
    return QString();
  else {
    bool success = false;

    if (filter == "BEMGEN files(*.txt)")
      success = saveMesh(plan, fileName, flat::BemgenMeshFormatter());
    else if (filter == "VTU files(*.vtu)")
      success = saveMesh(plan, fileName, flat::VTUMeshFormatter());

    return success? fileName : QString();
  }
}

bool FileManager::saveMesh(const flat::FlatMesh& mesh, const QString& fileName,
                           const flat::MeshFormatter& fmt) {
  std::ofstream output;
  output.open(fileName.toStdString());

  if (output.is_open()) {
    fmt.writeMesh(output, mesh);
    return true;
  }

  return false;
}
