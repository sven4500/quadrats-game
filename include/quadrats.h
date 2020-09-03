#pragma once
#ifndef QUADRATS_H_
#define QUADRATS_H_

#include <cassert>
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
#include <player.h>

class QuadratsGame : public QMainWindow
{

    Q_OBJECT

public:

    QuadratsGame(QWidget* parent = 0);
    ~QuadratsGame();

private:

    enum PlayerColor{PlayerOne = 0, PlayerTwo = 1};

    /*enum{BACK_COL = 0, LINE_COL, ACTLINE_COL, P1LINE_COL, P2LINE_COL};
    QColor const sm_colorProfile[] = {
        QColor(250, 254, 255), // BACK_COL
        QColor()
    };*/

    // Тип функции которая используется для отрисовки одного слоя изображения.
    typedef void (QuadratsGame::* PainterFunc)(QPainter&)const;

    static QColor const sm_backgroundColor;
    static QColor const sm_lineColor;
    static QColor const sm_activeLineColor;
    static QColor const sm_sideLineColor;

    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    unsigned int getOneSize()const; // возвращает рзмер одного квдрт в пикселях

    // Список методов отрисовки слоёв изображения.
    void paintBackgroud(QPainter& painter)const;
    void paintGrid(QPainter& painter)const;
    void paintGameBorder(QPainter& painter)const;
    void paintCurrentQuadrat(QPainter& painter)const;
    void paintCurrentLine(QPainter& painter)const;
    void paintCorner(QPainter& painter)const; // <= исчезнет после paintCaptured (просто добавляем по два поля каждому).
    void paintPlayerLines(QPainter& painter)const;
//    void paintCapturedQuadrats(QPainter& painter)const;

    // заполянет один квадрат заданным цветом
    void drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawLine(QPainter& painter, Line const& line, QColor const& color)const;
    void drawCross(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawCircle(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;

    // Возврщает индекс квадрата в глобальной системе координат (относительно левого верхнего угла).
    Quadrat getQuadrat(int x, int y)const;
    Quadrat translateQuadrat(Quadrat const& quadrat)const;

    // Возврщает линию квадрата в глобальной системе координат (относительно левого верхнего угла).
    Line getLine(int x, int y)const;
    Line translateLine(Line const& line)const;

    // Проверяет находится ли квадрат внутри игрового поля.
    bool isInside(Quadrat const& quadrat)const;
    bool isInside(Line const& line)const;

    // Вектор слоём отрисовки изображения.
    QVector<PainterFunc> m_painterFuncs;

    QColor m_playerOneColor;
    QColor m_playerTwoColor;

    QTimer* m_timer;

    Player m_stats[2];
    PlayerColor m_player;
    unsigned int m_dimFull; // размер игрового поля с учётом отступов
    unsigned int m_dim; // размер реального игрового поля
    int m_x;
    int m_y;

};

#endif
