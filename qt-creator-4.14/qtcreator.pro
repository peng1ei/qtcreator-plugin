include(qtcreator.pri)

#version check qt
!minQtVersion(5, 14, 0) {
    message("Cannot build $$IDE_DISPLAY_NAME with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.14.0.")
}

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = src
unix:!macx:!isEmpty(copydata):SUBDIRS += bin

contains(QT_ARCH, i386): ARCHITECTURE = x86
else: ARCHITECTURE = $$QT_ARCH

macx: PLATFORM = "mac"
else:win32: PLATFORM = "windows"
else:linux-*: PLATFORM = "linux-$${ARCHITECTURE}"
else: PLATFORM = "unknown"
