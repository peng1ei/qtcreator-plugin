#include "MainWindow.h"
#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginerroroverview.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>

#include <QApplication>
#include <QSettings>

using namespace ExtensionSystem;

using PluginSpecSet = QVector<PluginSpec *>;

static QSettings *createUserSettings()
{
    return new QSettings(QSettings::IniFormat, QSettings::UserScope,
                         QLatin1String("QtProject"),
                         QLatin1String("app"));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(PLUGIN_FRAMEWORK)
    QSettings *settings = createUserSettings();
    QSettings *globalSettings = new QSettings(QSettings::IniFormat, QSettings::SystemScope,
                                              QLatin1String("Plugin Framework"),
                                              QLatin1String("app"));

    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("org.qt-project.Qt.QtCreatorPlugin"));
    PluginManager::setGlobalSettings(globalSettings);
    PluginManager::setSettings(settings);

    const QStringList pluginPaths = {"D:/QtBuild/qtcreator/qtcreator-plugin/pluginframework/bin/debug/plugins"};
    PluginManager::setPluginPaths(pluginPaths);

    const PluginSpecSet plugins = PluginManager::plugins();
    PluginSpec *coreplugin = nullptr;
    for (PluginSpec *spec : plugins) {
        if (spec->name() == QLatin1String("MyPlugin")) {
            coreplugin = spec;
            break;
        }
    }

    if (!coreplugin) {
        qDebug() << "coreplugin failed";
        return 1;
    }

    if (coreplugin->hasError()) {
        qDebug() << "coreplugin->hasError()";
        return 1;
    }

    PluginManager::checkForProblematicPlugins();
    PluginManager::loadPlugins();
    if (coreplugin->hasError()) {
        qDebug() << "coreplugin->hasError()";
        return 1;
    }
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
