#ifndef FLATMESHER_BEMGENMESHFORMATTER_H_
#define FLATMESHER_BEMGENMESHFORMATTER_H_

#include "MeshFormatter.h"

namespace flat {

class BemgenMeshFormatter: public MeshFormatter {
public:
  virtual ~BemgenMeshFormatter() = default;
  virtual std::ostream& writeMesh(std::ostream& os, const Mesh& mesh) const;
  virtual std::istream& readMesh(std::istream& is, Mesh& mesh) const;

};

} // namespace flat

#endif // FLATMESHER_BEMGENMESHFORMATTER_H_
