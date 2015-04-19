#include "libprojprojectnodes.h"
#include "libprojprojectfile.h"
#include "libprojproject.h"
#include "libprojplugin.h"
#include <QFile>
#include "libprojprojectmanager.h"
#include "libproject.h"
#include "libproject_error.h"
#include <libgen.h>
#include <cstring>

typedef ProjectExplorer::Project AbstractProject;
using ProjectExplorer::FileNode;
using ProjectExplorer::FileType;
using Libproj::Internal::Plugin;
using ProjectExplorer::ProjectAction;
using LibprojManager::Internal::Project;
using LibprojManager::Interface::FileSetLoader;
using LibprojManager::Interface::Error::FileSetRuntimeError;
using std::vector;
using std::string;
using std::strstr;

namespace  LibprojManager {
namespace Internal {

ProjectNode::ProjectNode(AbstractProject * Project, ProjectFile * ProjectFile)
    : ProjectExplorer::ProjectNode(ProjectFile->filePath().toString()),
      project(Project),
      projectFile(ProjectFile)
{
    qDebug() << "Calling c-tor for ProjectNode";
    setDisplayName(projectFile->filePath().toFileInfo().completeBaseName());
}

QList<ProjectExplorer::ProjectAction> ProjectNode::supportedActions(Node *node) const
{
    qDebug() << "Calling ProjectNode::supportedActions(Node *node) const";
    //Q_UNUSED(node);
    return QList<ProjectAction>()
        << ProjectExplorer::ProjectAction::AddNewFile
        << ProjectExplorer::ProjectAction::AddSubProject
        << ProjectExplorer::ProjectAction::RemoveSubProject;
}

bool ProjectNode::canAddSubProject(const QString &proFilePath) const
{
    if (QFileInfo(proFilePath).suffix() == QString("libproject"))
        return true;
    return false;
}
bool ProjectNode::addSubProjects(const QStringList &proFilePaths)
{
    class subprojectIsntInParentFolder {
    };
    try {
        bool result = qobject_cast<Project*>(project)->addFiles(proFilePaths);
        if (result == false)
            return result;
        FileSetLoader * loader = qobject_cast<Project*>(project)->getLoader();
        vector<string> subprojectsPaths;
        for (const QString& filePath : proFilePaths)
        {
            char * dirc, * dname, * dircOfRoot, * dnameOfRoot;
            dirc = strdup(filePath.toLocal8Bit());
            dname = dirname(dirc);
            subprojectsPaths.push_back(filePath.toStdString());
            dircOfRoot = strdup(loader->getPathToNode().c_str());
            dnameOfRoot = dirname(dircOfRoot);
            char * found;
            found = strstr(dircOfRoot, dname);
            if (found == nullptr)
                throw subprojectIsntInParentFolder();
        }
        loader->addSubprojects(subprojectsPaths);
        loader->save();
        return true;
    } catch (const FileSetRuntimeError& re) {
        qWarning() << re.what();
        return false;
    } catch (subprojectIsntInParentFolder& ) {
        qWarning() << "This subproject isn't in parent folder";
        return false;
    }

}
bool ProjectNode::removeSubProjects(const QStringList &proFilePaths)
{
    //Manager * m = qobject_cast <Manager *>(project->projectManager());
    removeProjectNodes(qobject_cast<Project *>(project)->getSubprojectNodes());
    return true; //Err check TODO
}

bool ProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
    qDebug() << "Calling ProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)";
    Q_UNUSED(notAdded)
    return qobject_cast<Project*>(project)->addFiles(filePaths);
}

} // namespace Internal
} // namespace LibprojManager
