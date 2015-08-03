#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>

namespace flat {
class FlatMesh;
class FloorPlan;
class MeshFormatter;
}

class FileManager {
public:
  static QList<QPair<QString, flat::FloorPlan>> openFlats();
  static bool openFlat(const QString& fileName, flat::FloorPlan& plan);

  static QString saveFlat(const flat::FloorPlan &plan);
  static bool saveFlat(const flat::FloorPlan& plan, const QString& fileName);

  static QString saveMesh(const flat::FlatMesh &plan);
  static bool saveMesh(const flat::FlatMesh& mesh, const QString& fileName,
                       const flat::MeshFormatter& fmt);

// Avoid the creation of instances of this class by making the constructor private
private:
  FileManager() = default;

};

#endif // FILEMANAGER_H
