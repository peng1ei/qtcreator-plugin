/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "coreplugin.h"
///#include "designmode.h"
///#include "editmode.h"
///#include "helpmanager.h"
#include "icore.h"
///#include "idocument.h"
///#include "iwizardfactory.h"
#include "mainwindow.h"
///#include "modemanager.h"
////#include "reaper_p.h"
///#include "themechooser.h"

#include <coreplugin/actionmanager/actionmanager.h>
///#include <coreplugin/documentmanager.h>
///#include <coreplugin/editormanager/editormanager.h>
///#include <coreplugin/find/findplugin.h>
///#include <coreplugin/find/searchresultwindow.h>
///#include <coreplugin/locator/locator.h>
#include <coreplugin/coreconstants.h>
///#include <coreplugin/fileutils.h>

#include <app/app_version.h>
#include <extensionsystem/pluginerroroverview.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <utils/algorithm.h>
#include <utils/infobar.h>
#include <utils/macroexpander.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/pathchooser.h>
#include <utils/savefile.h>
#include <utils/stringutils.h>
#include <utils/theme/theme.h>
#include <utils/theme/theme_p.h>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include <cstdlib>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

static CorePlugin *m_instance = nullptr;

CorePlugin::CorePlugin()
{
    qRegisterMetaType<Id>();
    ///qRegisterMetaType<Core::Search::TextPosition>();
    qRegisterMetaType<Utils::CommandLine>();
    qRegisterMetaType<Utils::FilePath>();
    m_instance = this;
}

CorePlugin::~CorePlugin()
{
    ///IWizardFactory::destroyFeatureProvider();
    ///Find::destroy();

    ///delete m_locator;
    ///!delete m_editMode;

    ///DesignMode::destroyModeIfRequired();

    delete m_mainWindow;
    setCreatorTheme(nullptr);
}

CorePlugin *CorePlugin::instance()
{
    return m_instance;
}

struct CoreArguments {
    QColor overrideColor;
    Id themeId;
    bool presentationMode = false;
};

CoreArguments parseArguments(const QStringList &arguments)
{
    CoreArguments args;
    for (int i = 0; i < arguments.size(); ++i) {
        if (arguments.at(i) == QLatin1String("-color")) {
            const QString colorcode(arguments.at(i + 1));
            args.overrideColor = QColor(colorcode);
            i++; // skip the argument
        }
        if (arguments.at(i) == QLatin1String("-presentationMode"))
            args.presentationMode = true;
        if (arguments.at(i) == QLatin1String("-theme")) {
            args.themeId = Id::fromString(arguments.at(i + 1));
            i++; // skip the argument
        }
    }
    return args;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    new ActionManager(this);
    m_mainWindow = new MainWindow;

    std::srand(unsigned(QDateTime::currentDateTime().toSecsSinceEpoch()));
    m_mainWindow->init();

    return true;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
    if (ExtensionSystem::PluginManager::hasError()) {
        auto errorOverview = new ExtensionSystem::PluginErrorOverview(m_mainWindow);
        errorOverview->setAttribute(Qt::WA_DeleteOnClose);
        errorOverview->setModal(true);
        errorOverview->show();
    }
    checkSettings();
}

bool CorePlugin::delayedInitialize()
{
    ///m_locator->delayedInitialize();
    ///IWizardFactory::allWizardFactories(); // scan for all wizard factories
    return true;
}

QObject *CorePlugin::remoteCommand(const QStringList & /* options */,
                                   const QString &workingDirectory,
                                   const QStringList &args)
{
    if (!ExtensionSystem::PluginManager::isInitializationDone()) {
        connect(ExtensionSystem::PluginManager::instance(), &ExtensionSystem::PluginManager::initializationDone,
                this, [this, workingDirectory, args]() {
                    remoteCommand(QStringList(), workingDirectory, args);
        });
        return nullptr;
    }
///!    IDocument *res = m_mainWindow->openFiles(
///!                args, ICore::OpenFilesFlags(ICore::SwitchMode | ICore::CanContainLineAndColumnNumbers | ICore::SwitchSplitIfAlreadyVisible),
///!                workingDirectory);
    m_mainWindow->raiseWindow();
    return nullptr;// return res;
}

void CorePlugin::fileOpenRequest(const QString &f)
{
    remoteCommand(QStringList(), QString(), QStringList(f));
}

void CorePlugin::addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu)
{
//    QList<QAction*> actions = menu->actions();
//    QAction *firstAction = actions.isEmpty() ? nullptr : actions.first();

////    if (QDir().exists(pathChooser->filePath().toString())) {
////        auto *showInGraphicalShell = new QAction(Core::FileUtils::msgGraphicalShellAction(), menu);
////        connect(showInGraphicalShell, &QAction::triggered, pathChooser, [pathChooser]() {
////            Core::FileUtils::showInGraphicalShell(pathChooser, pathChooser->filePath().toString());
////        });
////        menu->insertAction(firstAction, showInGraphicalShell);

////        auto *showInTerminal = new QAction(Core::FileUtils::msgTerminalHereAction(), menu);
////        connect(showInTerminal, &QAction::triggered, pathChooser, [pathChooser]() {
////            if (pathChooser->openTerminalHandler())
////                pathChooser->openTerminalHandler()();
////            else
////                FileUtils::openTerminal(pathChooser->filePath().toString());
////        });
////        menu->insertAction(firstAction, showInTerminal);

//    } else {
//        auto *mkPathAct = new QAction(tr("Create Folder"), menu);
//        connect(mkPathAct, &QAction::triggered, pathChooser, [pathChooser]() {
//            QDir().mkpath(pathChooser->filePath().toString());
//            pathChooser->triggerChanged();
//        });
//        menu->insertAction(firstAction, mkPathAct);
//    }

//    if (firstAction)
//        menu->insertSeparator(firstAction);
}

void CorePlugin::checkSettings()
{
    const auto showMsgBox = [this](const QString &msg, QMessageBox::Icon icon) {
        connect(ICore::instance(), &ICore::coreOpened, this, [msg, icon]() {
            QMessageBox msgBox(ICore::dialogParent());
            msgBox.setWindowTitle(tr("Settings File Error"));
            msgBox.setText(msg);
            msgBox.setIcon(icon);
            msgBox.exec();
        }, Qt::QueuedConnection);
    };
    const QSettings * const userSettings = ICore::settings();
    QString errorDetails;
    switch (userSettings->status()) {
    case QSettings::NoError: {
        const QFileInfo fi(userSettings->fileName());
        if (fi.exists() && !fi.isWritable()) {
            const QString errorMsg = tr("The settings file \"%1\" is not writable.\n"
                    "You will not be able to store any %2 settings.")
                    .arg(QDir::toNativeSeparators(userSettings->fileName()),
                         QLatin1String(Core::Constants::IDE_DISPLAY_NAME));
            showMsgBox(errorMsg, QMessageBox::Warning);
        }
        return;
    }
    case QSettings::AccessError:
        errorDetails = tr("The file is not readable.");
        break;
    case QSettings::FormatError:
        errorDetails = tr("The file is invalid.");
        break;
    }
    const QString errorMsg = tr("Error reading settings file \"%1\": %2\n"
            "You will likely experience further problems using this instance of %3.")
            .arg(QDir::toNativeSeparators(userSettings->fileName()), errorDetails,
                 QLatin1String(Core::Constants::IDE_DISPLAY_NAME));
    showMsgBox(errorMsg, QMessageBox::Critical);
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    ///Find::aboutToShutdown();
    m_mainWindow->aboutToShutdown();
    return SynchronousShutdown;
}
