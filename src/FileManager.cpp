#include "FileManager.h"

#include "MessageManager.h"

#include <FlatMesher/BemgenMeshFormatter.h>
#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/VTUMeshFormatter.h>

#include <QFileDialog>

#include <fstream>
#include <string>

QList<QPair<QString, flat::FloorPlan>> FileManager::openFlats() {
  QStringList fileNames = QFileDialog::getOpenFileNames(nullptr,
                                                        QObject::tr("Open flat"),
                                                        QString(),
                                                        QObject::tr("FlatMesher flat files (*.flat);;All files(*)"));

  QList<QPair<QString, flat::FloorPlan>> result;
  for (QString fileName: fileNames) {
    if (!fileName.isNull()) {
      flat::FloorPlan plan;
      if (openFlat(fileName, plan))
        result << QPair<QString, flat::FloorPlan>(fileName, plan);
      else
        MessageManager::fileOpenFailed(nullptr, fileName);
    }
  }

  return result;
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
                                                  QObject::tr("FlatMesher flat files(*.flat);;All files(*)"));

  if (!fileName.isNull()) {
    if (!saveFlat(plan, fileName)) {
      MessageManager::fileSaveFailed(nullptr, fileName);
      return QString();
    }
  }

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

  if (!fileName.isNull()) {
    bool success = false;

    if (filter == QObject::tr("BEMGEN files(*.txt)"))
      success = saveMesh(plan, fileName, flat::BemgenMeshFormatter());
    else if (filter == QObject::tr("VTU files(*.vtu)"))
      success = saveMesh(plan, fileName, flat::VTUMeshFormatter());

    if (!success) {
      MessageManager::fileSaveFailed(nullptr, fileName);
      return QString();
    }
  }

  return fileName;
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
