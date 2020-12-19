#version check qt
#!minQtVersion(5, 14, 0) {
#    message("Cannot build $$IDE_DISPLAY_NAME with Qt version $${QT_VERSION}.")
#    error("Use at least Qt 5.14.0.")
#}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src
