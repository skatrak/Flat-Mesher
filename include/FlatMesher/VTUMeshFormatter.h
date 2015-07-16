#ifndef FLATMESHER_VTUMESHFORMATTER_H_
#define FLATMESHER_VTUMESHFORMATTER_H_

#include "MeshFormatter.h"

namespace flat {

class VTUMeshFormatter: public MeshFormatter {
public:
  virtual ~VTUMeshFormatter() = default;
  virtual std::ostream& writeMesh(std::ostream& os, const Mesh& mesh) const;
  virtual std::istream& readMesh(std::istream& is, Mesh& mesh) const;

};

} // namespace flat

#endif // FLATMESHER_VTUMESHFORMATTER_H_
