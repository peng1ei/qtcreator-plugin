include(../plugins.pri)

TEMPLATE = lib

DEFINES += MYPLUGIN_LIBRARY

QT += core widgets

#CONFIG(debug, debug|release){
#    DESTDIR = $$PWD/../../bin/debug/plugins

#    MOC_DIR = $$PWD/../../build/debug/moc
#    RCC_DIR = $$PWD/../../build/debug/rcc
#    UI_DIR = $$PWD/../../build/debug/ui
#    OBJECTS_DIR = $$PWD/../../build/debug/obj
#}else{
#    DESTDIR =$$PWD/../../bin/release

#    MOC_DIR = $$PWD/../../build/release/moc
#    RCC_DIR = $$PWD/../../build/release/rcc
#    UI_DIR = $$PWD/../../build/release/ui
#    OBJECTS_DIR = $$PWD/../../build/release/obj
#}

# MyPlugin files

SOURCES +=         mypluginplugin.cpp

HEADERS +=         mypluginplugin.h         myplugin_global.h         mypluginconstants.h

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
#isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
#isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "D:/QtBuild/qtcreator/pluginframework"

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "D:/QtBuild/qtcreator/pluginframework/build"

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = MyPlugin
QTC_LIB_DEPENDS +=     # nothing here at this time

#QTC_PLUGIN_DEPENDS +=     coreplugin

#QTC_PLUGIN_RECOMMENDS +=     # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

#INCLUDEPATH += $$PWD/../../
#LIBS += -L$${DESTDIR}/../ -lextensionsystem -laggregation -lutils

#include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
