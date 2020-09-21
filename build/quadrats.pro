QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += c++11

PROJECT_DIR = $$PWD/../
SRC_DIR = $$PROJECT_DIR/src/
INCLUDE_DIR = $$PROJECT_DIR/include/
RESOURCES_DIR = $$PROJECT_DIR/resources/
BUILDTMP_DIR = $$PROJECT_DIR/buildtmp/

Debug: {
    TARGET = QuadratsDebug
    DESTDIR = $$PROJECT_DIR/bin/
    DEFINES += _DEBUG
}

Release: {
    TARGET = Quadrats
    DESTDIR = $$PROJECT_DIR/bin/
    DEFINES += NDEBUG QT_NO_DEBUG QT_NO_DEBUG_OUTPUT
}

OBJECTS_DIR = $$BUILDTMP_DIR/
MOC_DIR = $$BUILDTMP_DIR/
RCC_DIR = $$BUILDTMP_DIR/

win32:RC_ICONS += $$RESOURCES_DIR/app.ico

INCLUDEPATH += $$INCLUDE_DIR/

SOURCES += \
    $$SRC_DIR/main.cpp \
    $$SRC_DIR/quadratsgame.cpp \
    $$SRC_DIR/transform.cpp \
    $$SRC_DIR/gamelogic.cpp \
    $$SRC_DIR/imagecomposer.cpp \
    $$SRC_DIR/settingsdialog.cpp \
    $$SRC_DIR/netcoop.cpp

HEADERS += \
    $$INCLUDE_DIR/quadratsgame.h \
    $$INCLUDE_DIR/quadrat.h \
    $$INCLUDE_DIR/line.h \
    $$INCLUDE_DIR/playerstats.h \
    $$INCLUDE_DIR/version.h \
    $$INCLUDE_DIR/transform.h \
    $$INCLUDE_DIR/gamelogic.h \
    $$INCLUDE_DIR/imagecomposer.h \
    $$INCLUDE_DIR/settingsdialog.h \
    $$INCLUDE_DIR/netcoop.h

RESOURCES += \
    $$RESOURCES_DIR/resources.qrc
