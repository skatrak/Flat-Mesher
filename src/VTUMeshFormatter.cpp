#include "FlatMesher/VTUMeshFormatter.h"
#include "FlatMesher/Mesh.h"

#include <vector>

using namespace flat;

std::ostream& VTUMeshFormatter::writeMesh(std::ostream& os, const Mesh& mesh) const {
  std::vector<Point3> nodes = mesh.getNodes();
  std::vector<IndexTriangle> triangles = mesh.getMesh();

  std::streamsize prec = os.precision();
  os.precision(15);

  os << "<?xml version=\"1.0\"?>\n"
     << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\">\n"
     << "  <UnstructuredGrid>\n"
     << "    <Piece NumberOfPoints=\"" << nodes.size() << "\" NumberOfCells=\"" << triangles.size() << "\">\n"
     << "      <Points>\n"
     << "        <DataArray type=\"Float32\" Name=\"points\" NumberOfComponents=\"3\" format=\"ascii\">\n";

  for (auto i = nodes.begin(); i != nodes.end(); ++i)
    os << "          " << *i << '\n';

  os << "        </DataArray>\n"
     << "      </Points>\n"
     << "      <Cells>\n"
     << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";

  for (auto i = triangles.begin(); i != triangles.end(); ++i)
    os << "          " << *i << '\n';

  os << "        </DataArray>\n"
     << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";

  for (size_t i = 1; i <= triangles.size(); ++i)
    os << "          " << 3 * i << '\n';

  os << "        </DataArray>\n"
     << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";

  for (size_t i = 0; i < triangles.size(); ++i)
    os << "          5\n";

  os << "        </DataArray>\n"
     << "      </Cells>\n"
     << "    </Piece>\n"
     << "  </UnstructuredGrid>\n"
     << "</VTKFile>\n";

  os.precision(prec);

  return os;
}

std::istream& VTUMeshFormatter::readMesh(std::istream& is, Mesh& mesh) const {
  // Not implemented
  is.setstate(std::ios::failbit);
  return is;
}
