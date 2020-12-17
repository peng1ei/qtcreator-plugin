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

#include "icore.h"
#include <app/app_version.h>
#include <extensionsystem/pluginmanager.h>

#include <utils/qtcassert.h>

#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QSysInfo>

/*!
    \namespace Core
    \inmodule QtCreator
    \brief The Core namespace contains all classes that make up the Core plugin
    which constitute the basic functionality of \QC.
*/

/*!
    \enum Core::FindFlag
    This enum holds the find flags.

    \value FindBackward
           Searches backwards.
    \value FindCaseSensitively
           Considers case when searching.
    \value FindWholeWords
           Finds only whole words.
    \value FindRegularExpression
           Uses a regular epression as a search term.
    \value FindPreserveCase
           Preserves the case when replacing search terms.
*/

/*!
    \enum Core::ICore::ContextPriority

    This enum defines the priority of additional contexts.

    \value High
           Additional contexts that have higher priority than contexts from
           Core::IContext instances.
    \value Low
           Additional contexts that have lower priority than contexts from
           Core::IContext instances.

    \sa Core::ICore::updateAdditionalContexts()
*/

/*!
    \enum Core::SaveSettingsReason
    \internal
*/

/*!
    \namespace Core::Internal
    \internal
*/

/*!
    \class Core::ICore
    \inheaderfile coreplugin/icore.h
    \inmodule QtCreator
    \ingroup mainclasses

    \brief The ICore class allows access to the different parts that make up
    the basic functionality of \QC.

    You should never create a subclass of this interface. The one and only
    instance is created by the Core plugin. You can access this instance
    from your plugin through instance().
*/

/*!
    \fn void Core::ICore::coreAboutToOpen()

    Indicates that all plugins have been loaded and the main window is about to
    be shown.
*/

/*!
    \fn void Core::ICore::coreOpened()
    Indicates that all plugins have been loaded and the main window is shown.
*/

/*!
    \fn void Core::ICore::saveSettingsRequested(Core::ICore::SaveSettingsReason reason)
    Signals that the user has requested that the global settings
    should be saved to disk for a \a reason.

    At the moment that happens when the application is closed, and on \uicontrol{Save All}.
*/

/*!
    \fn void Core::ICore::coreAboutToClose()
    Enables plugins to perform some pre-end-of-life actions.

    The application is guaranteed to shut down after this signal is emitted.
    It is there as an addition to the usual plugin lifecycle functions, namely
    \c IPlugin::aboutToShutdown(), just for convenience.
*/

/*!
    \fn void Core::ICore::contextAboutToChange(const QList<Core::IContext *> &context)
    Indicates that a new \a context will shortly become the current context
    (meaning that its widget got focus).
*/

/*!
    \fn void Core::ICore::contextChanged(const Core::Context &context)
    Indicates that a new \a context just became the current context. This includes the context
    from the focus object as well as the additional context.
*/

#include "mainwindow.h"

#include <utils/hostosinfo.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>

using namespace Core::Internal;
using namespace ExtensionSystem;
using namespace Utils;

namespace Core {

// The Core Singleton
static ICore *m_instance = nullptr;
static MainWindow *m_mainwindow = nullptr;

/*!
    Returns the pointer to the instance. Only use for connecting to signals.
*/
ICore *ICore::instance()
{
    return m_instance;
}

/*!
    Returns whether the new item dialog is currently open.
*/
bool ICore::isNewItemDialogRunning()
{
    return false;
}

/*!
    Returns the currently open new item dialog widget, or \c nullptr if there is none.

    \sa isNewItemDialogRunning()
    \sa showNewItemDialog()
*/
QWidget *ICore::newItemDialog()
{
    return nullptr;
}

/*!
    \internal
*/
ICore::ICore(MainWindow *mainwindow)
{
    m_instance = this;
    m_mainwindow = mainwindow;
    // Save settings once after all plugins are initialized:
    connect(PluginManager::instance(), &PluginManager::initializationDone,
            this, [] { ICore::saveSettings(ICore::InitializationDone); });
    connect(PluginManager::instance(), &PluginManager::testsFinished, [this] (int failedTests) {
        emit coreAboutToClose();
        if (failedTests != 0)
            qWarning("Test run was not successful: %d test(s) failed.", failedTests);
        QCoreApplication::exit(failedTests);
    });
}

/*!
    \internal
*/
ICore::~ICore()
{
    m_instance = nullptr;
    m_mainwindow = nullptr;
}

/*!
    Returns the text to use on buttons that open the options dialog.

    \sa showOptionsDialog()
    \sa msgShowOptionsDialogToolTip()
*/
QString ICore::msgShowOptionsDialog()
{
    return QCoreApplication::translate("Core", "Configure...", "msgShowOptionsDialog");
}

/*!
    Returns the tool tip to use on buttons that open the options dialog.

    \sa showOptionsDialog()
    \sa msgShowOptionsDialog()
*/
QString ICore::msgShowOptionsDialogToolTip()
{
    if (Utils::HostOsInfo::isMacHost())
        return QCoreApplication::translate("Core", "Open Preferences dialog.",
                                           "msgShowOptionsDialogToolTip (mac version)");
    else
        return QCoreApplication::translate("Core", "Open Options dialog.",
                                           "msgShowOptionsDialogToolTip (non-mac version)");
}

/*!
    Returns the application's main settings object.

    You can use it to retrieve or set application-wide settings
    (in contrast to session or project specific settings).

    If \a scope is \c QSettings::UserScope (the default), the
    settings will be read from the user's settings, with
    a fallback to global settings provided with \QC.

    If \a scope is \c QSettings::SystemScope, only the installation settings
    shipped with the current version of \QC will be read. This
    functionality exists for internal purposes only.

    \sa settingsDatabase()
*/
QSettings *ICore::settings(QSettings::Scope scope)
{
    if (scope == QSettings::UserScope)
        return PluginManager::settings();
    else
        return PluginManager::globalSettings();
}

/*!
    Returns the application's settings database.

    The settings database is meant as an alternative to the regular settings
    object. It is more suitable for storing large amounts of data. The settings
    are application wide.

    \sa SettingsDatabase
    \sa settings()
*/
SettingsDatabase *ICore::settingsDatabase()
{
    return m_mainwindow->settingsDatabase();
}

/*!
    Returns the locale string for the user interface language that is currently
    configured in \QC. Use this to install your plugin's translation file with
    QTranslator.
*/
QString ICore::userInterfaceLanguage()
{
    return qApp->property("qtc_locale").toString();
}

/*!
    Returns the absolute path that is used for resources like
    project templates and the debugger macros.

    This abstraction is needed to avoid platform-specific code all over
    the place, since on \macos, for example, the resources are part of the
    application bundle.

    \sa userResourcePath()
*/
QString ICore::resourcePath()
{
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + '/' + RELATIVE_DATA_PATH);
}

/*!
    Returns the absolute path in the users directory that is used for
    resources like project templates.

    Use this function for finding the place for resources that the user may
    write to, for example, to allow for custom palettes or templates.

    \sa resourcePath()
*/

QString ICore::userResourcePath()
{
    // Create qtcreator dir if it doesn't yet exist
    const QString configDir = QFileInfo(settings(QSettings::UserScope)->fileName()).path();
    const QString urp = configDir + '/' + QLatin1String(Constants::IDE_ID);

    if (!QFileInfo::exists(urp + QLatin1Char('/'))) {
        QDir dir;
        if (!dir.mkpath(urp))
            qWarning() << "could not create" << urp;
    }

    return urp;
}

/*!
    Returns a writable path that can be used for persistent cache files.
*/
QString ICore::cacheResourcePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

/*!
    Returns the path to resources written by the installer, for example
    pre-defined kits and toolchains.
*/
QString ICore::installerResourcePath()
{
    return QFileInfo(settings(QSettings::SystemScope)->fileName()).path() + '/' + Constants::IDE_ID;
}

/*!
    Returns the path to the plugins that are included in the \QC installation.

    \internal
*/
QString ICore::pluginPath()
{
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + '/' + RELATIVE_PLUGIN_PATH);
}

/*!
    Returns the path where user-specific plugins should be written.

    \internal
*/
QString ICore::userPluginPath()
{
    QString pluginPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if (Utils::HostOsInfo::isAnyUnixHost() && !Utils::HostOsInfo::isMacHost())
        pluginPath += "/data";
    pluginPath += '/' + QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR) + '/';
    pluginPath += QLatin1String(Utils::HostOsInfo::isMacHost() ? Core::Constants::IDE_DISPLAY_NAME
                                                               : Core::Constants::IDE_ID);
    pluginPath += "/plugins/";
    pluginPath += QString::number(IDE_VERSION_MAJOR) + '.' + QString::number(IDE_VERSION_MINOR)
                  + '.' + QString::number(IDE_VERSION_RELEASE);
    return pluginPath;
}

/*!
    Returns the path to the command line tools that are included in the \QC
    installation.
 */
QString ICore::libexecPath()
{
    return QDir::cleanPath(QApplication::applicationDirPath() + '/' + RELATIVE_LIBEXEC_PATH);
}

static QString clangIncludePath(const QString &clangVersion)
{
    return "/lib/clang/" + clangVersion + "/include";
}

/*!
    \internal
*/
QString ICore::clangIncludeDirectory(const QString &clangVersion,
                                     const QString &clangFallbackIncludeDir)
{
    QDir dir(libexecPath() + "/clang" + clangIncludePath(clangVersion));
    if (!dir.exists() || !QFileInfo(dir, "stdint.h").exists())
        dir = QDir(clangFallbackIncludeDir);
    return QDir::toNativeSeparators(dir.canonicalPath());
}

/*!
    \internal
*/
static QString clangBinary(const QString &binaryBaseName, const QString &clangBinDirectory)
{
    const QString hostExeSuffix(QTC_HOST_EXE_SUFFIX);
    QFileInfo executable(ICore::libexecPath() + "/clang/bin/" + binaryBaseName + hostExeSuffix);
    if (!executable.exists())
        executable = QFileInfo(clangBinDirectory + "/" + binaryBaseName + hostExeSuffix);
    return QDir::toNativeSeparators(executable.canonicalFilePath());
}

/*!
    \internal
*/
QString ICore::clangExecutable(const QString &clangBinDirectory)
{
    return clangBinary("clang", clangBinDirectory);
}

/*!
    \internal
*/
QString ICore::clangTidyExecutable(const QString &clangBinDirectory)
{
    return clangBinary("clang-tidy", clangBinDirectory);
}

/*!
    \internal
*/
QString ICore::clazyStandaloneExecutable(const QString &clangBinDirectory)
{
    return clangBinary("clazy-standalone", clangBinDirectory);
}

static QString compilerString()
{
#if defined(Q_CC_CLANG) // must be before GNU, because clang claims to be GNU too
    QString isAppleString;
#if defined(__apple_build_version__) // Apple clang has other version numbers
    isAppleString = QLatin1String(" (Apple)");
#endif
    return QLatin1String("Clang " ) + QString::number(__clang_major__) + QLatin1Char('.')
            + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    return QLatin1String("GCC " ) + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
    if (_MSC_VER > 1999)
        return QLatin1String("MSVC <unknown>");
    if (_MSC_VER >= 1920)
        return QLatin1String("MSVC 2019");
    if (_MSC_VER >= 1910)
        return QLatin1String("MSVC 2017");
    if (_MSC_VER >= 1900)
        return QLatin1String("MSVC 2015");
#endif
    return QLatin1String("<unknown compiler>");
}

/*!
    Returns a string with the IDE's name and version, in the form "\QC X.Y.Z".
    Use this for "Generated by" strings and similar tasks.
*/
QString ICore::versionString()
{
    QString ideVersionDescription;
    if (QLatin1String(Constants::IDE_VERSION_LONG) != QLatin1String(Constants::IDE_VERSION_DISPLAY))
        ideVersionDescription = tr(" (%1)").arg(QLatin1String(Constants::IDE_VERSION_LONG));
    return tr("%1 %2%3").arg(QLatin1String(Constants::IDE_DISPLAY_NAME),
                             QLatin1String(Constants::IDE_VERSION_DISPLAY),
                             ideVersionDescription);
}

/*!
    \internal
*/
QString ICore::buildCompatibilityString()
{
    return tr("Based on Qt %1 (%2, %3 bit)").arg(QLatin1String(qVersion()),
                                                 compilerString(),
                                                 QString::number(QSysInfo::WordSize));
}

/*!
    Returns the main window of the application.

    For dialog parents use dialogParent().

    \sa dialogParent()
*/
QMainWindow *ICore::mainWindow()
{
    return m_mainwindow;
}

/*!
    Returns a widget pointer suitable to use as parent for QDialogs.
*/
QWidget *ICore::dialogParent()
{
    QWidget *active = QApplication::activeModalWidget();
    if (!active)
        active = QApplication::activeWindow();
    if (!active || (active && active->windowFlags().testFlag(Qt::SplashScreen)))
        active = m_mainwindow;
    return active;
}

/*!
    \internal
*/
QStatusBar *ICore::statusBar()
{
    return m_mainwindow->statusBar();
}

/*!
    Raises and activates the window for \a widget. This contains workarounds
    for X11.
*/
void ICore::raiseWindow(QWidget *widget)
{
    if (!widget)
        return;
    QWidget *window = widget->window();
    if (window && window == m_mainwindow) {
        m_mainwindow->raiseWindow();
    } else {
        window->raise();
        window->activateWindow();
    }
}

/*!
    Opens files using \a arguments and \a flags like it would be
    done if they were given to \QC on the command line, or
    they were opened via \uicontrol File > \uicontrol Open.
*/

void ICore::openFiles(const QStringList &arguments, ICore::OpenFilesFlags flags)
{
    Q_UNUSED(arguments)
    Q_UNUSED(flags)
}

/*!
    Provides a hook for plugins to veto on closing the application.

    When the application window requests a close, all listeners are called. If
    one of the \a listener calls returns \c false, the process is aborted and
    the event is ignored. If all calls return \c true, coreAboutToClose()
    is emitted and the event is accepted or performed.
*/
void ICore::addPreCloseListener(const std::function<bool ()> &listener)
{
    m_mainwindow->addPreCloseListener(listener);
}

/*!
    \internal
*/
QString ICore::systemInformation()
{
    QString result = PluginManager::instance()->systemInformation() + '\n';
    result += versionString() + '\n';
    result += buildCompatibilityString() + '\n';
#ifdef IDE_REVISION
    result += QString("From revision %1\n").arg(QString::fromLatin1(Constants::IDE_REVISION_STR).left(10));
#endif
#ifdef QTC_SHOW_BUILD_DATE
     result += QString("Built on %1 %2\n").arg(QLatin1String(__DATE__), QLatin1String(__TIME__));
#endif
     return result;
}

static const QByteArray &screenShotsPath()
{
    static const QByteArray path = qgetenv("QTC_SCREENSHOTS_PATH");
    return path;
}

class ScreenShooter : public QObject
{
public:
    ScreenShooter(QWidget *widget, const QString &name, const QRect &rc)
        : m_widget(widget), m_name(name), m_rc(rc)
    {
        m_widget->installEventFilter(this);
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QTC_ASSERT(watched == m_widget, return false);
        if (event->type() == QEvent::Show)
            QTimer::singleShot(0, this, &ScreenShooter::helper);
        return false;
    }

    void helper()
    {
        if (m_widget) {
            QRect rc = m_rc.isValid() ? m_rc : m_widget->rect();
            QPixmap pm = m_widget->grab(rc);
            for (int i = 0; ; ++i) {
                QString fileName = screenShotsPath() + '/' + m_name + QString("-%1.png").arg(i);
                if (!QFileInfo::exists(fileName)) {
                    pm.save(fileName);
                    break;
                }
            }
        }
        deleteLater();
    }

    QPointer<QWidget> m_widget;
    QString m_name;
    QRect m_rc;
};

/*!
    \internal
*/
void ICore::setupScreenShooter(const QString &name, QWidget *w, const QRect &rc)
{
    if (!screenShotsPath().isEmpty())
        new ScreenShooter(w, name, rc);
}

/*!
    Restarts \QC and restores the last session.
*/
void ICore::restart()
{
    m_mainwindow->restart();
}

/*!
    \internal
*/
void ICore::saveSettings(SaveSettingsReason reason)
{
    emit m_instance->saveSettingsRequested(reason);
    m_mainwindow->saveSettings();

    ICore::settings(QSettings::SystemScope)->sync();
    ICore::settings(QSettings::UserScope)->sync();
}

/*!
    \internal
*/
QStringList ICore::additionalAboutInformation()
{
    return m_mainwindow->additionalAboutInformation();
}

/*!
    \internal
*/
void ICore::appendAboutInformation(const QString &line)
{
    m_mainwindow->appendAboutInformation(line);
}

void ICore::updateNewItemDialogState()
{
    static bool wasRunning = false;
    static QWidget *previousDialog = nullptr;
    if (wasRunning == isNewItemDialogRunning() && previousDialog == newItemDialog())
        return;
    wasRunning = isNewItemDialogRunning();
    previousDialog = newItemDialog();
    emit instance()->newItemDialogStateChanged();
}

} // namespace Core
