#include <fstream>
#include <iostream>

#include <FlatMesher/FloorPlan.h>

int main(int argc, char* argv[]) {
  std::ifstream in;
  std::ofstream out;

  in.open("test/test1.flat");
  out.open("test/test1_gen.flat");

  if (in.is_open() && out.is_open()) {
    flat::FloorPlan plan;
    in >> plan;
    in.close();

    out << plan;
    out.close();
  }
  else {
    std::cerr << "Error trying to open the files\n";
    return 1;
  }

  return 0;
}
