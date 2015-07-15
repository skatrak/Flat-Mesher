Compilation steps:
  cd build
  CXX='g++ -std=c++11 -fopenmp' cmake ..
  make

Execution (from the "build" directory):
  ./FlatMesher ../test/test1.flat mesh.txt
