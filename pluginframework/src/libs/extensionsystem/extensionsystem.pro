include(../libs.pri)
QT += core gui widgets

TEMPLATE = lib
DEFINES += EXTENSIONSYSTEM_LIBRARY

#CONFIG += c++17 c++1z
#CONFIG += shared dll

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG(debug, debug|release){
#    DESTDIR = $$PWD/../bin/debug

#    MOC_DIR = $$PWD/../build/debug/moc
#    RCC_DIR = $$PWD/../build/debug/rcc
#    UI_DIR = $$PWD/../build/debug/ui
#    OBJECTS_DIR = $$PWD/../build/debug/obj
#}else{
#    DESTDIR =$$PWD/../bin/release

#    MOC_DIR = $$PWD/../build/release/moc
#    RCC_DIR = $$PWD/../build/release/rcc
#    UI_DIR = $$PWD/../build/release/ui
#    OBJECTS_DIR = $$PWD/../build/release/obj
#}

#INCLUDEPATH += $$PWD/../
#INCLUDEPATH += $$PWD/../aggregation
#INCLUDEPATH += $$PWD/../utils

LIBS += -L$${DESTDIR} -laggregation -lutils

#CONFIG(debug, debug|release){
#    LIBS += -L$${DESTDIR} -laggregation
#    LIBS += -L$${DESTDIR} -lutils
#}else{
#    LIBS += -L$${DESTDIR} -laggregation
#    LIBS += -L$${DESTDIR} -lutils
#}

SOURCES +=  \
    invoker.cpp \
    iplugin.cpp \
    optionsparser.cpp \
    plugindetailsview.cpp \
    pluginerroroverview.cpp \
    pluginerrorview.cpp \
    pluginmanager.cpp \
    pluginspec.cpp \
    pluginview.cpp

HEADERS += \
    extensionsystem_global.h \
    invoker.h \
    iplugin.h \
    iplugin_p.h \
    optionsparser.h \
    plugindetailsview.h \
    pluginerroroverview.h \
    pluginerrorview.h \
    pluginmanager.h \
    pluginmanager_p.h \
    pluginspec.h \
    pluginspec_p.h \
    pluginview.h \
    ui_plugindetailsview.h \
    ui_pluginerroroverview.h \
    ui_pluginerrorview.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    plugindetailsview.ui \
    pluginerroroverview.ui \
    pluginerrorview.ui
