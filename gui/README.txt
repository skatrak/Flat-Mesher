First compile the FlatMesher library. It will build the 'libFlatMesher.a' static library that
is linked to the GUI application.

Compilation steps (console):
  cd build
  qmake ../FlatMesherGUI.pro
  make

Compilation steps (QtCreator):
  File > Open file or project...
  Select the 'FlatMesherGUI.pro' file
  Press Ctrl+B to build or Ctrl+R to run
