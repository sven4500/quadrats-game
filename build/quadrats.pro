QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

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
    $$SRC_DIR/quadrats_graphics.cpp \
    $$SRC_DIR/quadrats_main.cpp

HEADERS += \
    $$INCLUDE_DIR/quadrats.h

RESOURCES += \
    $$RESOURCES_DIR/resources.qrc
