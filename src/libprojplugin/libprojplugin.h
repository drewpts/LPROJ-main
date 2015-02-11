#pragma once

#include "libproj_global.h"
#include <extensionsystem/iplugin.h>
#include <projectexplorer/project.h>
#include "libprojproject.h"
#include <QVector>
#include "libprojprojectnodes.h"
#include "../../tools/json11/json11.hpp"

class QFile;
namespace Libproj {
namespace Internal {


class LibprojPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "libproj.json")

public:
    LibprojPlugin();
    virtual ~LibprojPlugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();
    virtual ShutdownFlag aboutToShutdown();

private:
    virtual QString readProjectFile();
    virtual bool parseMetadata(const QString & strJson);
private slots:
    virtual void triggerOpenProjectAction();
    virtual void triggerAddNewFileAction();

    //members
private:
    bool isReadOnly;
    bool erroneousState;

    static json11::Json projectData;
    static QVector<QFile *> files;
    QString projectFilename;
    QString er;
    ProjectExplorer::Project * project;

    void saveProjectData(const json11::Json & changedProjectData);

    friend class LibprojProjectManager::Internal::OwnProject;
    friend void LibprojProjectManager::Internal::OwnProjectNode::addFileNodes(const QVariantMap& Data, const QFileInfo &fileInfo);

};

} // namespace Internal
} // namespace Libproj
