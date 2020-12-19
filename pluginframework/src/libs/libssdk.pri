include(../../pluginframework.pri)

message("PF_APP_BIN_DIR libssdk: $${PF_APP_BIN_DIR}")

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/aggregation
INCLUDEPATH += $$PWD/utils
INCLUDEPATH += $$PWD/extensionsystem

LIBS += -L$${PF_APP_BIN_DIR}/libs -lextensionsystem -laggregation -lutils
