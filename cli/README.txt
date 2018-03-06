The FlatMesher library is also compiled when compiling the CLI application, so there's no need to
compile it before this.

Compilation instructions:
  cd build/
  cmake ..
  make

How to run:
  ./FlatMesherCLI {<input_file> [{-f | --format} {vtu | bemgen}] [{-o | --output} <output_file>] | {-h | --help}}
