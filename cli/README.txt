For CMake to be able to find the FlatMesher library, it has to be inside a folder called 'FlatMesher' in
the parent directory of this directory.

The FlatMesher library is also compiled when doing this, so there's no need to compile it before this.

Compilation instructions:
  cd build/
  cmake ..
  make

How to run:
  ./FlatMesherCLI {<input_file> [{-f | --format} {vtu | bemgen}] [{-o | --output} <output_file>] | {-h | --help}}
