#ifndef FLATMESHER_MESHFORMATTER_H_
#define FLATMESHER_MESHFORMATTER_H_

#include <iostream>

namespace flat {

class Mesh;

class MeshFormatter {
public:
  virtual ~MeshFormatter() = default;
  virtual std::ostream& writeMesh(std::ostream& os, const Mesh& mesh) const = 0;
  virtual std::istream& readMesh(std::istream& is, Mesh& mesh) const = 0;

};

} // namespace flat

#endif // FLATMESHER_MESHFORMATTER_H_
