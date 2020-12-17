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

#include "mainwindow.h"

#include "icore.h"
#include "plugindialog.h"
#include "versiondialog.h"
#include "coreicons.h"

#include <app/app_version.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actionmanager_p.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/settingsdatabase.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>
#include <utils/hostosinfo.h>
#include <utils/qtcassert.h>
#include <utils/stylehelper.h>
#include <utils/stringutils.h>
#include <utils/utilsicons.h>

#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

using namespace ExtensionSystem;
using namespace Utils;

namespace Core {
namespace Internal {

enum { debugMainWindow = 0 };

MainWindow::MainWindow()
    : AppMainWindow()
    , m_coreImpl(new ICore(this))
    , m_settingsDatabase(
          new SettingsDatabase(QFileInfo(PluginManager::settings()->fileName()).path(),
                               QLatin1String(Constants::IDE_CASED_ID),
                               this))
{
    setWindowTitle(Constants::IDE_DISPLAY_NAME);
    if (HostOsInfo::isLinuxHost())
        QApplication::setWindowIcon(Icons::QTCREATORLOGO_BIG.icon());
    QString baseName = QApplication::style()->objectName();
    // Sometimes we get the standard windows 95 style as a fallback
    if (HostOsInfo::isAnyUnixHost() && !HostOsInfo::isMacHost()
            && baseName == QLatin1String("windows")) {
        baseName = QLatin1String("fusion");
    }

    setDockNestingEnabled(true);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    registerDefaultContainers();
    registerDefaultActions();

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);

    auto dropSupport = new DropSupport(this, [](QDropEvent *event, DropSupport *) {
        return event->source() == nullptr; // only accept drops from the "outside" (e.g. file manager)
    });
    connect(dropSupport, &DropSupport::filesDropped,
            this, &MainWindow::openDroppedFiles);
}

bool MainWindow::askConfirmationBeforeExit() const
{
    return m_askConfirmationBeforeExit;
}

void MainWindow::setAskConfirmationBeforeExit(bool ask)
{
    m_askConfirmationBeforeExit = ask;
}

void MainWindow::setOverrideColor(const QColor &color)
{
    m_overrideColor = color;
}

QStringList MainWindow::additionalAboutInformation() const
{
    return m_aboutInformation;
}

void MainWindow::appendAboutInformation(const QString &line)
{
    m_aboutInformation.append(line);
}

void MainWindow::addPreCloseListener(const std::function<bool ()> &listener)
{
    m_preCloseListeners.append(listener);
}

MainWindow::~MainWindow()
{
    delete m_coreImpl;
    m_coreImpl = nullptr;
}

void MainWindow::init()
{
}

void MainWindow::extensionsInitialized()
{
    readSettings();
    emit m_coreImpl->coreAboutToOpen();

    // Delay restoreWindowState, since it is overridden by LayoutRequest event
    QTimer::singleShot(0, this, &MainWindow::restoreWindowState); // show window
    QTimer::singleShot(0, m_coreImpl, &ICore::coreOpened);
}

static void setRestart(bool restart)
{
    qApp->setProperty("restart", restart);
}

void MainWindow::restart()
{
    setRestart(true);
    exit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const auto cancelClose = [event] {
        event->ignore();
        setRestart(false);
    };

    // work around QTBUG-43344
    static bool alreadyClosed = false;
    if (alreadyClosed) {
        event->accept();
        return;
    }

    if (m_askConfirmationBeforeExit &&
            (QMessageBox::question(this,
                                   tr("Exit %1?").arg(Constants::IDE_DISPLAY_NAME),
                                   tr("Exit %1?").arg(Constants::IDE_DISPLAY_NAME),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No)
             == QMessageBox::No)) {
        event->ignore();
        return;
    }

    ICore::saveSettings(ICore::MainWindowClosing);

    foreach (const std::function<bool()> &listener, m_preCloseListeners) {
        if (!listener()) {
            cancelClose();
            return;
        }
    }

    emit m_coreImpl->coreAboutToClose();

    saveWindowSettings();

    event->accept();
    alreadyClosed = true;
}

void MainWindow::openDroppedFiles(const QList<DropSupport::FileSpec> &files)
{
    raiseWindow();
    QStringList filePaths = Utils::transform(files, &DropSupport::FileSpec::filePath);
}

QStatusBar *MainWindow::statusBar() const
{
    return nullptr;
}

void MainWindow::registerDefaultContainers()
{
    ActionContainer *menubar = ActionManager::createMenuBar(Constants::MENU_BAR);

    if (!HostOsInfo::isMacHost()) // System menu bar on Mac
        setMenuBar(menubar->menuBar());
    //menubar->appendGroup(Constants::G_FILE);
    //menubar->appendGroup(Constants::G_EDIT);
    //menubar->appendGroup(Constants::G_VIEW);
    //menubar->appendGroup(Constants::G_TOOLS);
    //menubar->appendGroup(Constants::G_WINDOW);
    menubar->appendGroup(Constants::G_HELP);

#if 0
    // File Menu
    ActionContainer *filemenu = ActionManager::createMenu(Constants::M_FILE);
    menubar->addMenu(filemenu, Constants::G_FILE);
    filemenu->menu()->setTitle(tr("&File"));
    filemenu->appendGroup(Constants::G_FILE_NEW);
    filemenu->appendGroup(Constants::G_FILE_OPEN);
    filemenu->appendGroup(Constants::G_FILE_PROJECT);
    filemenu->appendGroup(Constants::G_FILE_SAVE);
    filemenu->appendGroup(Constants::G_FILE_EXPORT);
    filemenu->appendGroup(Constants::G_FILE_CLOSE);
    filemenu->appendGroup(Constants::G_FILE_PRINT);
    filemenu->appendGroup(Constants::G_FILE_OTHER);
    connect(filemenu->menu(), &QMenu::aboutToShow, this, &MainWindow::aboutToShowRecentFiles);


    // Edit Menu
    ActionContainer *medit = ActionManager::createMenu(Constants::M_EDIT);
    ///menubar->addMenu(medit, Constants::G_EDIT);
    medit->menu()->setTitle(tr("&Edit"));
    medit->appendGroup(Constants::G_EDIT_UNDOREDO);
    medit->appendGroup(Constants::G_EDIT_COPYPASTE);
    medit->appendGroup(Constants::G_EDIT_SELECTALL);
    medit->appendGroup(Constants::G_EDIT_ADVANCED);
    medit->appendGroup(Constants::G_EDIT_FIND);
    medit->appendGroup(Constants::G_EDIT_OTHER);
#endif

#if 0
    ActionContainer *mview = ActionManager::createMenu(Constants::M_VIEW);
    menubar->addMenu(mview, Constants::G_VIEW);
    mview->menu()->setTitle(tr("&View"));
    mview->appendGroup(Constants::G_VIEW_VIEWS);
    mview->appendGroup(Constants::G_VIEW_PANES);

    // Tools Menu
    ActionContainer *ac = ActionManager::createMenu(Constants::M_TOOLS);
    menubar->addMenu(ac, Constants::G_TOOLS);
    ac->menu()->setTitle(tr("&Tools"));

    // Window Menu
//    ActionContainer *mwindow = ActionManager::createMenu(Constants::M_WINDOW);
//    menubar->addMenu(mwindow, Constants::G_WINDOW);
//    mwindow->menu()->setTitle(tr("&Window"));
//    mwindow->appendGroup(Constants::G_WINDOW_SIZE);
//    mwindow->appendGroup(Constants::G_WINDOW_SPLIT);
//    mwindow->appendGroup(Constants::G_WINDOW_NAVIGATE);
//    mwindow->appendGroup(Constants::G_WINDOW_LIST);
//    mwindow->appendGroup(Constants::G_WINDOW_OTHER);
#endif

    // Help Menu
    ActionContainer *ac = ActionManager::createMenu(Constants::M_HELP);
    menubar->addMenu(ac, Constants::G_HELP);
    ac->menu()->setTitle(tr("&Help"));
    ac->appendGroup(Constants::G_HELP_HELP);
    //ac->appendGroup(Constants::G_HELP_SUPPORT);
    ac->appendGroup(Constants::G_HELP_ABOUT);
    //ac->appendGroup(Constants::G_HELP_UPDATES);

#if 0
    // macOS touch bar
//    ac = ActionManager::createTouchBar(Constants::TOUCH_BAR,
//                                       QIcon(),
//                                       "Main TouchBar" /*never visible*/);
//    ac->appendGroup(Constants::G_TOUCHBAR_HELP);
//    ac->appendGroup(Constants::G_TOUCHBAR_EDITOR);
//    ac->appendGroup(Constants::G_TOUCHBAR_NAVIGATION);
//    ac->appendGroup(Constants::G_TOUCHBAR_OTHER);
//    ac->touchBar()->setApplicationTouchBar();
#endif
}

void MainWindow::registerDefaultActions()
{
    //ActionContainer *mfile = ActionManager::actionContainer(Constants::M_FILE);
    //ActionContainer *medit = ActionManager::actionContainer(Constants::M_EDIT);
    //ActionContainer *mview = ActionManager::actionContainer(Constants::M_VIEW);
    //ActionContainer *mtools = ActionManager::actionContainer(Constants::M_TOOLS);
    //ActionContainer *mwindow = ActionManager::actionContainer(Constants::M_WINDOW);
    ActionContainer *mhelp = ActionManager::actionContainer(Constants::M_HELP);

#if 0
    // File menu separators
//    mfile->addSeparator(Constants::G_FILE_SAVE);
//    mfile->addSeparator(Constants::G_FILE_EXPORT);
//    mfile->addSeparator(Constants::G_FILE_PRINT);
//    mfile->addSeparator(Constants::G_FILE_CLOSE);
//    mfile->addSeparator(Constants::G_FILE_OTHER);

    // Return to editor shortcut: Note this requires Qt to fix up
    // handling of shortcut overrides in menus, item views, combos....
    //m_focusToEditor = new QAction(tr("Return to Editor"), this);
    Command *cmd;// = ActionManager::registerAction(m_focusToEditor, Constants::S_RETURNTOEDITOR);
    //cmd->setDefaultKeySequence(QKeySequence(Qt::Key_Escape));
    //connect(m_focusToEditor, &QAction::triggered, this, &MainWindow::setFocusToEditor);

    // New File Action
    QIcon icon;
//    m_newAction = new QAction(icon, tr("&New File or Project..."), this);
//    cmd = ActionManager::registerAction(m_newAction, Constants::NEW);
//    cmd->setDefaultKeySequence(QKeySequence::New);
//    mfile->addAction(cmd, Constants::G_FILE_NEW);
//    connect(m_newAction, &QAction::triggered, this, []() {
//        if (!ICore::isNewItemDialogRunning()) {
//        } else {
//            ICore::raiseWindow(ICore::newItemDialog());
//        }
//    });

    // Open Action
    //icon;
//    m_openAction = new QAction(icon, tr("&Open File or Project..."), this);
//    cmd = ActionManager::registerAction(m_openAction, Constants::OPEN);
//    cmd->setDefaultKeySequence(QKeySequence::Open);
//    mfile->addAction(cmd, Constants::G_FILE_OPEN);
//    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);

    // Open With Action
//    m_openWithAction = new QAction(tr("Open File &With..."), this);
//    cmd = ActionManager::registerAction(m_openWithAction, Constants::OPEN_WITH);
//    mfile->addAction(cmd, Constants::G_FILE_OPEN);
//    connect(m_openWithAction, &QAction::triggered, this, &MainWindow::openFileWith);

    // File->Recent Files Menu
//    ActionContainer *ac = ActionManager::createMenu(Constants::M_FILE_RECENTFILES);
//    mfile->addMenu(ac, Constants::G_FILE_OPEN);
//    ac->menu()->setTitle(tr("Recent &Files"));
//    ac->setOnAllDisabledBehavior(ActionContainer::Show);


    // Save Action
//    icon;
    QAction *tmpaction;

//    // Exit Action
//    icon = QIcon::fromTheme(QLatin1String("application-exit"));
//    m_exitAction = new QAction(icon, tr("E&xit"), this);
//    m_exitAction->setMenuRole(QAction::QuitRole);
//    cmd = ActionManager::registerAction(m_exitAction, Constants::EXIT);
//    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Q")));
//    mfile->addAction(cmd, Constants::G_FILE_OTHER);
//    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exit);

    // Full Screen Action
    QAction *toggleFullScreenAction = new QAction(tr("Full Screen"), this);
    toggleFullScreenAction->setCheckable(!HostOsInfo::isMacHost());
    toggleFullScreenAction->setEnabled(true); // actual implementation in WindowSupport
    cmd = ActionManager::registerAction(toggleFullScreenAction, Constants::TOGGLE_FULLSCREEN);
    cmd->setDefaultKeySequence(QKeySequence(useMacShortcuts ? tr("Ctrl+Meta+F") : tr("Ctrl+Shift+F11")));
    if (HostOsInfo::isMacHost())
        cmd->setAttribute(Command::CA_UpdateText);
    //mwindow->addAction(cmd, Constants::G_WINDOW_SIZE);

    registerModeSelectorStyleActions();

    // Window->Views
    ///ActionContainer *mviews = ActionManager::createMenu(Constants::M_VIEW_VIEWS);
    ///mview->addMenu(mviews, Constants::G_VIEW_VIEWS);
    ///mviews->menu()->setTitle(tr("&Views"));

#endif

    // "Help" separators
    mhelp->addSeparator(Constants::G_HELP_SUPPORT);
    if (!HostOsInfo::isMacHost())
        mhelp->addSeparator(Constants::G_HELP_ABOUT);

    mhelp->setOnAllDisabledBehavior(ActionContainer::OnAllDisabledBehavior::Show);

    Command *cmd = nullptr;
    QAction *tmpaction = nullptr;

    // About IDE Action
    QIcon icon = QIcon::fromTheme(QLatin1String("help-about"));
    if (HostOsInfo::isMacHost())
        tmpaction = new QAction(icon, tr("About &%1").arg(Constants::IDE_DISPLAY_NAME), this); // it's convention not to add dots to the about menu
    else
        tmpaction = new QAction(icon, tr("About &%1...").arg(Constants::IDE_DISPLAY_NAME), this);
    tmpaction->setMenuRole(QAction::AboutRole);
    cmd = ActionManager::registerAction(tmpaction, Constants::ABOUT_QTCREATOR);
    mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    tmpaction->setEnabled(true);
    connect(cmd->action(), &QAction::triggered, this, &MainWindow::aboutQtCreator);
    cmd->action()->setEnabled(true);

    //About Plugins Action
    tmpaction = new QAction(tr("About &Plugins..."), this);
    tmpaction->setMenuRole(QAction::ApplicationSpecificRole);
    cmd = ActionManager::registerAction(tmpaction, Constants::ABOUT_PLUGINS);
    mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    tmpaction->setEnabled(true);
    connect(cmd->action(), &QAction::triggered, this, &MainWindow::aboutPlugins);
    cmd->action()->setEnabled(true);

    // About Qt Action
//    tmpaction = new QAction(tr("About &Qt..."), this);
//    cmd = ActionManager::registerAction(tmpaction, Constants:: ABOUT_QT);
//    mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
//    tmpaction->setEnabled(true);
//    connect(tmpaction, &QAction::triggered, qApp, &QApplication::aboutQt);

    // About sep
    if (!HostOsInfo::isMacHost()) { // doesn't have the "About" actions in the Help menu
        tmpaction = new QAction(this);
        tmpaction->setSeparator(true);
        cmd = ActionManager::registerAction(tmpaction, "QtCreator.Help.Sep.About");
        mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    }
}

void MainWindow::registerModeSelectorStyleActions()
{
    ActionContainer *mview = ActionManager::actionContainer(Constants::M_VIEW);

    // Cycle Mode Selector Styles
    m_cycleModeSelectorStyleAction = new QAction(tr("Cycle Mode Selector Styles"), this);
    ActionManager::registerAction(m_cycleModeSelectorStyleAction, Constants::CYCLE_MODE_SELECTOR_STYLE);

    // Mode Selector Styles
    ActionContainer *mmodeLayouts = ActionManager::createMenu(Constants::M_VIEW_MODESTYLES);
    mview->addMenu(mmodeLayouts, Constants::G_VIEW_VIEWS);
    QMenu *styleMenu = mmodeLayouts->menu();
    styleMenu->setTitle(tr("Mode Selector Style"));
    auto *stylesGroup = new QActionGroup(styleMenu);
    stylesGroup->setExclusive(true);

    m_setModeSelectorStyleIconsAndTextAction = stylesGroup->addAction(tr("Icons and Text"));
    m_setModeSelectorStyleIconsAndTextAction->setCheckable(true);
    m_setModeSelectorStyleIconsOnlyAction = stylesGroup->addAction(tr("Icons Only"));
    m_setModeSelectorStyleIconsOnlyAction->setCheckable(true);
    m_setModeSelectorStyleHiddenAction = stylesGroup->addAction(tr("Hidden"));
    m_setModeSelectorStyleHiddenAction->setCheckable(true);

    styleMenu->addActions(stylesGroup->actions());
}

void MainWindow::openFile()
{
}

void MainWindow::setFocusToEditor()
{
}

void MainWindow::exit()
{
    // this function is most likely called from a user action
    // that is from an event handler of an object
    // since on close we are going to delete everything
    // so to prevent the deleting of that object we
    // just append it
    QTimer::singleShot(0, this,  &QWidget::close);
}

void MainWindow::openFileWith()
{
}

void MainWindow::updateFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)

    // Prevent changing the context object just because the menu or a menu item is activated
    if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
        return;
}

void MainWindow::aboutToShutdown()
{
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);
    hide();
}

static const char settingsGroup[] = "MainWindow";
static const char colorKey[] = "Color";
static const char askBeforeExitKey[] = "AskBeforeExit";
static const char windowGeometryKey[] = "WindowGeometry";
static const char windowStateKey[] = "WindowState";
static const char modeSelectorLayoutKey[] = "ModeSelectorLayout";

void MainWindow::readSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    if (m_overrideColor.isValid()) {
        StyleHelper::setBaseColor(m_overrideColor);
        // Get adapted base color.
        m_overrideColor = StyleHelper::baseColor();
    } else {
        StyleHelper::setBaseColor(settings->value(QLatin1String(colorKey),
                                  QColor(StyleHelper::DEFAULT_BASE_COLOR)).value<QColor>());
    }

    m_askConfirmationBeforeExit = settings->value(askBeforeExitKey, false).toBool();

    {
        // Migrate legacy setting from Qt Creator 4.6 and earlier
        static const char modeSelectorVisibleKey[] = "ModeSelectorVisible";
        if (!settings->contains(modeSelectorLayoutKey) && settings->contains(modeSelectorVisibleKey)) {
            //bool visible = settings->value(modeSelectorVisibleKey, true).toBool();
        }

        updateModeSelectorStyleMenu();
    }

    settings->endGroup();
}

void MainWindow::saveSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    if (!(m_overrideColor.isValid() && StyleHelper::baseColor() == m_overrideColor))
        settings->setValue(QLatin1String(colorKey), StyleHelper::requestedBaseColor());

    settings->setValue(askBeforeExitKey, m_askConfirmationBeforeExit);

    settings->endGroup();

    ActionManager::saveSettings();
}

void MainWindow::saveWindowSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    // On OS X applications usually do not restore their full screen state.
    // To be able to restore the correct non-full screen geometry, we have to put
    // the window out of full screen before saving the geometry.
    // Works around QTBUG-45241
    if (Utils::HostOsInfo::isMacHost() && isFullScreen())
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    settings->setValue(QLatin1String(windowGeometryKey), saveGeometry());
    settings->setValue(QLatin1String(windowStateKey), saveState());

    settings->endGroup();
}

void MainWindow::updateModeSelectorStyleMenu()
{
}

void MainWindow::aboutToShowRecentFiles()
{
    return;
}

void MainWindow::aboutQtCreator()
{
    if (!m_versionDialog) {
        m_versionDialog = new VersionDialog(this);
        connect(m_versionDialog, &QDialog::finished,
                this, &MainWindow::destroyVersionDialog);
        m_versionDialog->show();
    } else {
        ICore::raiseWindow(m_versionDialog);
    }
}

void MainWindow::destroyVersionDialog()
{
    if (m_versionDialog) {
        m_versionDialog->deleteLater();
        m_versionDialog = nullptr;
    }
}

void MainWindow::aboutPlugins()
{
    PluginDialog dialog(this);
    dialog.exec();
}

void MainWindow::restoreWindowState()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));
    if (!restoreGeometry(settings->value(QLatin1String(windowGeometryKey)).toByteArray()))
        resize(1260, 700); // size without window decoration
    restoreState(settings->value(QLatin1String(windowStateKey)).toByteArray());
    settings->endGroup();
    show();
}

} // namespace Internal
} // namespace Core
