include(../libs/libssdk.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++17 c++1z

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += PLUGIN_FRAMEWORK

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = $${PF_APP_BIN_DIR}

# TODO move to pluginframework.pri
MOC_DIR = $${PF_MOC_DIR}
RCC_DIR = $${PF_RCC_DIR}
UI_DIR = $${PF_UI_DIR}
OBJECTS_DIR = $${PF_OBJECTS_DIR}

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
#LIBS += -L$${DESTDIR} -lextensionsystem -laggregation -lutils

SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
