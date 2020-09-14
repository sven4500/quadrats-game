#pragma once
#ifndef QUADRATS_H_
#define QUADRATS_H_

#include <QMainWindow>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QRect>
#include <QColor>
#include <QSize>
#include <QMouseEvent>
#include <QMenuBar>
#include <QDebug>
#include <QTimer>
#include <quadrat.h>
#include <line.h>
#include <playerstats.h>
#include <gamelogic.h>
#include <imagecomposer.h>
#include <settingsdialog.h>

class QuadratsGame: public QMainWindow
{
    Q_OBJECT

public:

    QuadratsGame(QWidget* parent = 0);
    ~QuadratsGame();

private:
    typedef GameLogic::Player PlayerEnum;

    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    Quadrat getQuadratCentral()const;

    // Возвращает рзмер одного квдрта в пикселях.
    unsigned int getOneSize()const;

    GameLogic m_logic;
    ImageComposer m_composer;

    SettingsDialog m_settingsDialog;

    QTimer m_timer;

    QAction* m_createGameAct;
    QAction* m_joinGameAct;
    QAction* m_settingsAct;
    QAction* m_howToAct;
    QAction* m_aboutAct;

    // Размер игрового поля с учётом отступов и размер игрового поля. Имеется
    // ограничение. Размер игрового поля с отступами должен быть чётным, а
    // размер игрового поля нечётным. Также значение m_dimFull должно всегда
    // быть больше либо равено m_dim.
    int m_dimFull;
    int m_dim;

    int m_x;
    int m_y;

};

#endif
