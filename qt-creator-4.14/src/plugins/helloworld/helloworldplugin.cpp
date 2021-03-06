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

#include "helloworldplugin.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>

#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>

namespace HelloWorld {
namespace Internal {


/*! Constructs the Hello World plugin. Normally plugins don't do anything in
    their constructor except for initializing their member variables. The
    actual work is done later, in the initialize() and extensionsInitialized()
    functions.
*/
HelloWorldPlugin::HelloWorldPlugin()
{
}

/*! Plugins are responsible for deleting objects they created on the heap, and
    to unregister objects from the plugin manager that they registered there.
*/
HelloWorldPlugin::~HelloWorldPlugin()
{

}

/*! Initializes the plugin. Returns true on success.
    Plugins want to register objects with the plugin manager here.

    \a errorMessage can be used to pass an error message to the plugin system,
       if there was any.
*/
bool HelloWorldPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

#if 1
    // Create an action to be triggered by a menu entry
    auto helloWorldAction = new QAction(tr("Say \"&Hello World!\""), this);
    helloWorldAction->setObjectName("hello");
    connect(helloWorldAction, &QAction::triggered, this, &HelloWorldPlugin::sayHelloWorld);

    // Register the action with the action manager
    Core::Command *command =
            Core::ActionManager::registerAction(
                    helloWorldAction, "HelloWorld.HelloWorldAction");

    // Create our own menu to place in the Tools menu
    Core::ActionContainer *helloWorldMenu =
            Core::ActionManager::createMenu("HelloWorld.HelloWorldMenu");
    QMenu *menu = helloWorldMenu->menu();
    menu->setTitle(tr("&Hello World"));
    menu->setEnabled(true);

    // Add the Hello World action command to the menu
    helloWorldMenu->addAction(command);

    // Request the Tools menu and add the Hello World menu to it
    Core::ActionContainer *toolsMenu =
            Core::ActionManager::createMenu("MyPlugin");
            //Core::ActionManager::actionContainer("MyPlugin"/*Core::Constants::M_TOOLS*/);
    toolsMenu->addMenu(helloWorldMenu);
    toolsMenu->menu()->setTitle("&MyPlugin");

    Core::ActionContainer *menubar = Core::ActionManager::createMenuBar(Core::Constants::MENU_BAR);
    menubar->appendGroup("MyPlugin");

    // 1. add to first
    //menubar->addMenu(toolsMenu, "MyPlugin"/*Group*/);

    // 2. add to M_HELP before, if "before == nullptr", will insert last.
    QMenu *beforeMenu = Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu();
    menubar->menuBar()->insertMenu(beforeMenu->menuAction(), toolsMenu->menu());

    qDebug() << "helloWorldAction: " << helloWorldAction;
    qDebug() << "command: " << command->action()->objectName();
    //connect(helloWorldAction, &QAction::triggered, this, &HelloWorldPlugin::sayHelloWorld);
    //connect(command->action(), &QAction::triggered, this, &HelloWorldPlugin::sayHelloWorld);
    //command->action()->setEnabled(true);

    // Add a mode with a push button based on BaseMode.
#else
    Core::ActionContainer *menubar = Core::ActionManager::createMenuBar(Core::Constants::MENU_BAR);

    // Create a DoNothing menu
    Core::ActionContainer* ac = am->createMenu("DoNothingPlugin.DoNothingMenu");
    ac->menu()->setTitle("DoNothing");
    // Create a command for "About DoNothing".
    Core::Command* cmd = am->registerAction(
       new QAction(this),
       "DoNothingPlugin.AboutDoNothing");
    cmd->action()->setText("About DoNothing");
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(about()));
    // Add DoNothing menu to the menubar
    am->actionContainer(Core::Constants::MENU_BAR)->addMenu(ac);
    // Add the "About DoNothing" action to the DoNothing menu
    ac->addAction(cmd);

    menubar->addMenu();
#endif


    return true;
}

/*! Notification that all extensions that this plugin depends on have been
    initialized. The dependencies are defined in the plugins .json(.in) file.

    Normally this function is used for things that rely on other plugins to have
    added objects to the plugin manager, that implement interfaces that we're
    interested in. These objects can now be requested through the
    PluginManager.

    The HelloWorldPlugin doesn't need things from other plugins, so it does
    nothing here.
*/
void HelloWorldPlugin::extensionsInitialized()
{
    return;
    QMessageBox::information(
            nullptr, tr("Hello World!"), tr("extensionsInitialized!!!"));
}

void HelloWorldPlugin::sayHelloWorld()
{
    // When passing nullptr for the parent, the message box becomes an
    // application-global modal dialog box
    QMessageBox::information(
            nullptr, tr("Hello World!"), tr("Hello World! Beautiful day today, isn't it?"));
}

} // namespace Internal
} // namespace HelloWorld
