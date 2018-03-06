#include "MessageManager.h"

#include "Configuration.h"

#include <QMessageBox>

void MessageManager::fileSaveFailed(QWidget *parent, const QString& fileName) {
  QMessageBox::critical(parent, QObject::tr("File save failed"),
                        QObject::tr("There was an error when saving the file \"%1\". "
                                    "Make sure you have the write permissions in that directory/file.").arg(fileName));
}

void MessageManager::fileOpenFailed(QWidget *parent, const QString& fileName) {
  QMessageBox::critical(parent, QObject::tr("File open failed"),
                        QObject::tr("There was an error while opening the file \"%1\". "
                                    "Make sure you have read permissions for that file and it is not corrupted.").arg(fileName));
}

void MessageManager::invalidMeshExport(QWidget *parent) {
  QMessageBox::critical(parent, QObject::tr("Mesh export failed"),
                        QObject::tr("There was an error when saving the mesh. "
                                    "The 2D plan has errors. Check <i>Tools &gt; Find problems...</i>"));
}

void MessageManager::aboutApplication(QWidget *parent) {
  QMessageBox::about(parent, QObject::tr("About"),
                     QObject::tr("<b>FlatMesher</b><tr/>"
                                 "Version %1. Copyright Sergio M. Afonso Fumero 2015.").arg(config::VERSION_STRING));
}

bool MessageManager::fileReload(QWidget *parent, const QString& fileName) {
  return QMessageBox::question(parent, QObject::tr("Reload file"),
                               QObject::tr("The file \"%1\" has unsaved changes."
                                           "Do you want to reload the model?"
                                           "All changes will be lost.").arg(fileName),
                               QMessageBox::Yes | QMessageBox::No,
                               QMessageBox::No) == QMessageBox::Yes;
}

int MessageManager::fileUnsavedChanges(QWidget *parent, const QString& fileName) {
  return QMessageBox::question(parent, QObject::tr("Save changes"),
                               QObject::tr("The file \"%1\" has unsaved changes. Do you want to save it?").arg(fileName),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                               QMessageBox::Save);
}
