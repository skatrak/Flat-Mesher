#include <fstream>
#include <iostream>

#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/BemgenMeshFormatter.h>
#include <FlatMesher/VTUMeshFormatter.h>

void printUsage(char* program_name) {
  std::cout << "Usage: " << program_name << " <input_file> <output_file>\n";
}

int main(int argc, char* argv []) {
  if (argc != 3) {
    printUsage(argv[0]);
    return 1;
  }

  std::ifstream in(argv[1]);
  if (!in.is_open()) {
    std::cerr << "Input file \"" << argv[1] << "\" could not be opened.\n";
    return 1;
  }

  std::ofstream out(argv[2]);
  if (!out.is_open()) {
    std::cerr << "Output file \"" << argv[1] << "\" could not be opened.\n";
    return 1;
  }

  flat::FloorPlan plan;

  if (!(in >> plan)) {
    std::cerr << "The input file doesn't have a correct format.\n";
    return 1;
  }
  in.close();

  if (!plan.valid()) {
    std::cerr << "The input plan doesn't represent a valid map of the building.\n";
    return 1;
  }

  flat::FlatMesh mesh;
  flat::VTUMeshFormatter fmt;

  mesh.createFromPlan(&plan);
  fmt.writeMesh(out, mesh);

  out.close();

  std::cout << "Mesh generated successfully at \"" << argv[2] << "\".";
  return 0;
}
