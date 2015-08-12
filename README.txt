First compile the FlatMesher library. You will obtain a 'libFlatMesher.a' file.
Copy that file into the `libs/` directory.

Compilation steps (console):
  cd build
  qmake ../FlatMesherGUI.pro
  make

Compilation steps (QtCreator):
  File > Open file or project...
  Select the 'FlatMesherGUI.pro' file
  Press Ctrl+B to build or Ctrl+R to run
