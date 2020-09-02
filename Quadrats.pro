QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

Debug: {
    TARGET = QuadratsDebug
    DEFINES += _DEBUG
}

Release: {
    TARGET = QuadratsRelease
    DEFINES += NDEBUG QT_NO_DEBUG QT_NO_DEBUG_OUTPUT
}

win32:RC_ICONS += app.ico

SOURCES += main.cpp\
    quadrats_graphics.cpp \
    quadrats_main.cpp

HEADERS  += \
    quadrats.h

RESOURCES += \
    resource.qrc
