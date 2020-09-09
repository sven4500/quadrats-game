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
#include <playerstats.h>

class QuadratsGame : public QMainWindow
{
    Q_OBJECT

public:

    QuadratsGame(QWidget* parent = 0);
    ~QuadratsGame();

private:

    enum PlayerEnum{PlayerOne = 0, PlayerTwo = 1, P1 = 0, P2 = 1};

    // Тип функции которая используется для отрисовки одного слоя изображения.
    typedef void (QuadratsGame::* PainterFunc)(QPainter&)const;

    static QColor const sm_backgroundColor;
    static QColor const sm_gridColor;

    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    // Список методов отрисовки слоёв изображения.
    void paintBackgroud(QPainter& painter)const;
    void paintGrid(QPainter& painter)const;
    void paintGameBorder(QPainter& painter)const;
    void paintCurrentQuadrat(QPainter& painter)const;
    void paintCurrentLine(QPainter& painter)const;
    void paintPlayerLines(QPainter& painter)const;
    void paintCapturedQuadrats(QPainter& painter)const;
    void paintScore(QPainter& painter)const;

    // заполянет один квадрат заданным цветом
    void drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color, Qt::BrushStyle style = Qt::SolidPattern)const;
    void drawQuadratBorders(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawCapturedQuadrat(QPainter& painter, Quadrat const&, PlayerEnum player)const;
    void drawLine(QPainter& painter, Line const& line, QColor const& color)const;
    void drawPlayerInsignia(QPainter& painter, Quadrat const& quadrat, PlayerEnum player)const;
    void drawCross(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawCircle(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawPlayerScore(QPainter& painter, PlayerEnum player)const;

    // Возврщает индекс квадрата в глобальной системе координат (относительно
    // левого верхнего угла). Перегружены методы для преобразования квадратов
    // между системами координат.
    Quadrat getQuadratGlobal(int x, int y)const;
    Quadrat toLocal(Quadrat const& quadrat)const;
    Quadrat toGlobal(Quadrat const& quadrat)const;

    // Возврщает линию квадрата в глобальной системе координат (относительно
    // левого верхнего угла). Перегружены два метода для преобразования линии в
    // глобальную и относительную системы координат.
    Line getLineGlobal(int x, int y)const;
    Line toLocal(Line const& line)const;
    Line toGlobal(Line const& line)const;

    void addInitialStats();
    void addPaintRoutines();

    // Возвращает рзмер одного квдрта в пикселях.
    unsigned int getOneSize()const;

    bool tryToEnclose(Quadrat const& quadrat)const;
    bool tryToEnclose(Line const& line);

    // Проверяет находится ли квадрат внутри игрового поля.
    bool isInside(Quadrat const& quadrat)const;
    bool isInside(Line const& line)const;
    bool isPlayerAcquired(Line const& line)const;

    // Вектор слоёв отрисовки изображения.
    QVector<PainterFunc> m_painterFuncs;

    QTimer m_timer;

    PlayerStats m_stats[2];
    PlayerEnum m_currentPlayer;

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
