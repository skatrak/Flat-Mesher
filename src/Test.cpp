#include <fstream>
#include <iostream>

#include <FlatMesher/FloorPlan.h>

void runTest(bool(*test_func)(), const char* test_name) {
  if (!test_func())
    std::cerr << "Test: " << test_name << " FAILED!\n";
}

// Global variables
const char* input_file = "test/test1.flat";
const char* output_file = "test/test1_gen.flat";

// Tests
bool testReadWriteFloorPlan();
bool testIsValidFloorPlan();

int main(int argc, char* argv[]) {
  runTest(testReadWriteFloorPlan, "Read/Write input file");
  runTest(testIsValidFloorPlan, "isValid floor plan");

  return 0;
}

bool testReadWriteFloorPlan() {
  std::ifstream in;
  std::ofstream out;

  in.open(input_file);
  out.open(output_file);

  if (in.is_open() && out.is_open()) {
    flat::FloorPlan plan1, plan2;
    in >> plan1;
    in.close();

    out << plan1;
    out.close();

    in.open(output_file);
    if (in.is_open()) {
      in >> plan2;
      in.close();

      // We compare both plans to make sure they are equal
      if (plan1.getNodes() != plan2.getNodes() ||
          plan1.getHeight() != plan2.getHeight() ||
          plan1.getTriangleSize() != plan2.getTriangleSize()) {
        std::cerr << "The copied file does not have the same contents than the original one\n";
        return false;
      }
    }
    else {
      std::cerr << "The output file cannot be accessed\n";
      return false;
    }
  }
  else {
    std::cerr << "Error trying to open the files\n";
    return false;
  }

  return true;
}

bool testIsValidFloorPlan() {
  std::ifstream in(input_file);
  if (in.is_open()) {
    flat::FloorPlan plan;

    in >> plan;
    in.close();

    std::cout << (plan.valid()? "Valid" : "Invalid") << " plan\n";
  }
  else {
    std::cerr << "Error trying to open the file\n";
    return false;
  }

  return true;
}