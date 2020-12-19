include(../../pluginframework.pri)

FP_LIBS_DIR = $${PF_APP_BIN_DIR}/libs
message("FP_LIBS_DIR libs: $${FP_LIBS_DIR}")

DESTDIR = $${FP_LIBS_DIR}

CONFIG += shared dll

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/aggregation
INCLUDEPATH += $$PWD/utils
