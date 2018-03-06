#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include <FlatMesher/FloorPlan.h>
#include <FlatMesher/FlatMesh.h>
#include <FlatMesher/BemgenMeshFormatter.h>
#include <FlatMesher/VTUMeshFormatter.h>

enum class RunMode {
  ERROR = -1,
  DEFAULT,
  GENERATE,
  HELP
};

enum class OutputFormat {
  ERROR = -1,
  BEMGEN,
  VTU
};

struct program_input_t {
  RunMode mode;
  std::string in_file, out_file;
  OutputFormat out_format;
};

const char* DEFAULT_OUTPUT_FILE_NAME = "tmp.vtu";

inline bool streq(const char* str1, const char* str2) {
  return std::strcmp(str1, str2) == 0;
}

void printUsage(char* program_name) {
  std::cout << "Usage: " << program_name
    << " {<input_file> [{-f | --format} {vtu | bemgen}] [{-o | --output} <output_file>] | {-h | --help}}\n";
}

program_input_t processArgs(int argc, char* argv []);
bool generate(program_input_t input);

int main(int argc, char *argv []) {
  program_input_t params = processArgs(argc, argv);

  switch (params.mode) {
  case RunMode::ERROR:
    std::cerr << "Incorrect arguments passed in.\n";
    printUsage(argv[0]);
    return 1;
  case RunMode::GENERATE:
    return !generate(params);
  case RunMode::DEFAULT:
  case RunMode::HELP:
    printUsage(argv[0]);
  default:
    return 0;
  }
}

program_input_t processArgs(int argc, char* argv []) {
  // flatmesher {input.flat [-f {vtu|bemgen}] [-o output.flat] | -h}
  program_input_t info;
  info.out_file = DEFAULT_OUTPUT_FILE_NAME;
  info.out_format = OutputFormat::VTU;

  if (argc < 2)
    info.mode = RunMode::DEFAULT;
  else if (argc == 2 && (streq(argv[1], "-h") || streq(argv[1], "--help")))
    info.mode = RunMode::HELP;
  else {
    info.mode = RunMode::GENERATE;
    info.in_file = argv[1];

    for (int i = 2; i < argc - 1; i += 2) {
      if (streq(argv[i], "-f") || streq(argv[i], "--format")) {
        if (streq(argv[i + 1], "bemgen"))
          info.out_format = OutputFormat::BEMGEN;
        else if (streq(argv[i + 1], "vtu"))
          info.out_format = OutputFormat::VTU;
        else
          info.out_format = OutputFormat::ERROR;
      }
      else if (streq(argv[i], "-o") || streq(argv[i], "--output")) {
        info.out_file = argv[i + 1];
      }
      else {
        info.mode = RunMode::ERROR;
        break;
      }
    }
  }

  return info;
}

bool generate(program_input_t input) {
  std::ifstream in(input.in_file.c_str());
  if (!in.is_open()) {
    std::cerr << "Input file \"" << input.in_file << "\" could not be opened.\n";
    return false;
  }

  std::ofstream out(input.out_file.c_str());
  if (!out.is_open()) {
    std::cerr << "Output file \"" << input.out_file << "\" could not be opened.\n";
    return false;
  }

  flat::FloorPlan plan;

  if (!(in >> plan)) {
    std::cerr << "The input file doesn't have a correct format.\n";
    return false;
  }
  in.close();

  if (!plan.valid()) {
    std::cerr << "The input plan doesn't represent a valid map of the building.\n";
    return false;
  }

  flat::FlatMesh mesh;
  flat::MeshFormatter* fmt = nullptr;

  switch (input.out_format) {
  case OutputFormat::ERROR:
    return false;
  case OutputFormat::BEMGEN:
    fmt = new flat::BemgenMeshFormatter;
    break;
  case OutputFormat::VTU:
    fmt = new flat::VTUMeshFormatter;
    break;
  }

  mesh.createFromPlan(&plan);
  fmt->writeMesh(out, mesh);

  delete fmt;
  out.close();

  std::cout << "Mesh generated successfully at \"" << input.out_file << "\".\n";
  return true;
}
