#-------------------------------------------------
#
# Project created by QtCreator 2017-08-11T13:20:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32:RC_ICONS += app.ico

TARGET = Quadrats
TEMPLATE = app

CONFIG(debug, debug|release): DEFINES += _DEBUG
CONFIG(release, debug|release): DEFINES += NDEBUG

SOURCES += main.cpp\
    quadrats_graphics.cpp \
    quadrats_main.cpp

HEADERS  += \
    quadrats.h

RESOURCES += \
    resource.qrc
