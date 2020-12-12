TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS += \
    #shared \
    libs \
    app \
    plugins
    #tools \
    #share/qtcreator/data.pro \
    #share/3rdparty/data.pro

# delegate deployqt target
deployqt.CONFIG += recursive
#deployqt.recurse = shared libs app plugins tools
deployqt.recurse = libs app plugins
QMAKE_EXTRA_TARGETS += deployqt
