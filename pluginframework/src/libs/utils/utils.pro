include(../libs.pri)

QT -= gui
QT += core widgets

TEMPLATE = lib
DEFINES += UTILS_LIBRARY

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

win32: LIBS += -luser32 -lshell32
# PortsGatherer
#win32: LIBS += -liphlpapi -lws2_32

SOURCES += \
    benchmarker.cpp \
    categorysortfiltermodel.cpp \
    environment.cpp \
    fileutils.cpp \
    hostosinfo.cpp \
    itemviews.cpp \
    namevaluedictionary.cpp \
    namevalueitem.cpp \
    qtcassert.cpp \
    qtcprocess.cpp \
    savefile.cpp \
    stringutils.cpp \
    synchronousprocess.cpp \
    treemodel.cpp

HEADERS += \
    algorithm.h \
    benchmarker.h \
    categorysortfiltermodel.h \
    environment.h \
    environmentfwd.h \
    executeondestruction.h \
    fileutils.h \
    hostosinfo.h \
    itemviews.h \
    namevaluedictionary.h \
    namevalueitem.h \
    optional.h \
    osspecificaspects.h \
    porting.h \
    predicates.h \
    qtcassert.h \
    qtcprocess.h \
    savefile.h \
    stringutils.h \
    synchronousprocess.h \
    treemodel.h \
    utils_global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
