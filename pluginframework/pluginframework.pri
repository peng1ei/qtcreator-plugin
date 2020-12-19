CONFIG(debug, debug|release){
    PF_APP_BIN_DIR = $$PWD/bin/debug

	PF_BUILD_DIR = $$PWD/build/debug
    PF_MOC_DIR = $${PF_BUILD_DIR}/moc
    PF_RCC_DIR = $${PF_BUILD_DIR}/rcc
    PF_UI_DIR = $${PF_BUILD_DIR}/ui
    PF_OBJECTS_DIR = $${PF_BUILD_DIR}/obj
}else{
    PF_APP_BIN_DIR = $$PWD/bin/release

	PF_BUILD_DIR = $$PWD/build/release
    PF_MOC_DIR = $${PF_BUILD_DIR}/moc
    PF_RCC_DIR = $${PF_BUILD_DIR}/rcc
    PF_UI_DIR = $${PF_BUILD_DIR}/ui
    PF_OBJECTS_DIR = $${PF_BUILD_DIR}/obj
}

CONFIG += c++17 c++1z
