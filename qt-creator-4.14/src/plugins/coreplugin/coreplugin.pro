DEFINES += CORE_LIBRARY
QT += \
    network \
    printsupport \
    #qml \
   sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../qtcreatorplugin.pri)
msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += \
    mainwindow.cpp \
    icontext.cpp \
    versiondialog.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/command.cpp \
    actionmanager/commandbutton.cpp \
    actionmanager/actioncontainer.cpp \
    actionmanager/commandsfile.cpp \
    coreplugin.cpp \
    plugindialog.cpp \
    icore.cpp \
    settingsdatabase.cpp \
    dialogs/restartdialog.cpp \
    windowsupport.cpp \
    messagebox.cpp \
    coreicons.cpp \
    plugininstallwizard.cpp

HEADERS += \
    mainwindow.h \
    actionmanager/actioncontainer.h \
    actionmanager/actionmanager.h \
    actionmanager/command.h \
    actionmanager/commandbutton.h \
    actionmanager/actionmanager_p.h \
    actionmanager/command_p.h \
    actionmanager/actioncontainer_p.h \
    actionmanager/commandsfile.h \
    icontext.h \
    icore.h \
    coreconstants.h \
    versiondialog.h \
    core_global.h \
    coreplugin.h \
    plugindialog.h \
    settingsdatabase.h \
    dialogs/restartdialog.h \
    windowsupport.h \
    messagebox.h \
    coreicons.h \
    plugininstallwizard.h

RESOURCES += core.qrc

win32 {
    #SOURCES += progressmanager/progressmanager_win.cpp
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    #OBJECTIVE_SOURCES += \
        #progressmanager/progressmanager_mac.mm
    LIBS += -framework AppKit
}
else:unix {
    #SOURCES += progressmanager/progressmanager_x11.cpp

    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = images/logo/$${imagesize}/QtProject-qtcreator.png)
        eval(image$${imagesize}.path = $$QTC_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}

DEFINES -= WITH_TESTS
