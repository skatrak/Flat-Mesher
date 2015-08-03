#ifndef MESSAGEMANAGER
#define MESSAGEMANAGER

class QString;
class QWidget;

class MessageManager {
public:
  static void fileSaveFailed(QWidget *parent, const QString& fileName);
  static void fileOpenFailed(QWidget *parent, const QString& fileName);
  static void invalidMeshExport(QWidget *parent);

  static void aboutApplication(QWidget *parent);

  static bool fileReload(QWidget *parent, const QString& fileName);
  static int fileUnsavedChanges(QWidget *parent, const QString& fileName);

private:
  MessageManager() {}

};

#endif // MESSAGEMANAGER
