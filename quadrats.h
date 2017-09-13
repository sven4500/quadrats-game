#ifndef __CQUADRATS_H
#define __CQUADRATS_H

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
#include <cassert>

class CQuadrats : public QMainWindow
{

    Q_OBJECT

public:

    CQuadrats(QWidget* parent = 0);
    ~CQuadrats();

private:

    enum Player{PlayerOne = 0, PlayerTwo = 1};

    struct QUADRAT
    {
        // Система отсчёта квадратов. Относительно левого верхнего угла (Global),
        // либо относительно центра игрового поля (Local).
        enum Origin{Local = 0, Global = 1};

        QUADRAT(): origin(Global), x(0), y(0)
        {}

        QUADRAT(int x, int y, Origin origin = Global): origin(origin), x(x), y(y)
        {}

        bool isLocal()const
        {
            return (origin == Origin::Local) ? true : false;
        }

        bool isGlobal()const
        {
            return (origin == Origin::Global) ? true : false;
        }

        Origin origin;
        int x:16;
        int y:16;
    };

    // структура хранит информацию об отдельной линии
    struct LINE
    {
        enum Orientation{Unknown = -1, Left = 0, Up, Right, Down};

        LINE(): orient(Unknown)
        {
            quadrat.x = -1;
            quadrat.y = -1;
        }

        LINE(int x, int y, Orientation orientation): orient(orientation)
        {
            quadrat.x = x;
            quadrat.y = y;
        }

        bool isValid()const
        {
            return orient != Unknown;
        }

        bool isLocal()const
        {
            return quadrat.isLocal();
        }

        bool isGlobal()const
        {
            return quadrat.isGlobal();
        }

        Orientation orient; // какая сторона квадрата
        QUADRAT quadrat; // какой квадрат
    };

    // структура хранит ходы одного игрока
    struct PLAYER_STATS
    {
        QVector<LINE> lines;
        QVector<QUADRAT> captured;
        QColor playerColor;
    };

    /*enum{BACK_COL = 0, LINE_COL, ACTLINE_COL, P1LINE_COL, P2LINE_COL};
    QColor const sm_colorProfile[] = {
        QColor(250, 254, 255), // BACK_COL
        QColor()
    };*/

    static QColor const sm_backgroundColor;
    static QColor const sm_lineColor;
    static QColor const sm_activeLineColor;
    static QColor const sm_sideLineColor;

    QColor m_playerOneColor;
    QColor m_playerTwoColor;
    //static QColor const sm_activeLineColor;

    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    //virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    unsigned int getOneSize()const; // возвращает рзмер одного квдрт в пикселях

    void paintBackground(QPainter& painter)const;
    void paintBorder(QPainter& painter)const;
    void paintCurrentQuadrat(QPainter& painter)const;
    void paintCurrentLine(QPainter& painter)const;
    void paintCorner(QPainter& painter)const; // <= исчезнет после paintCaptured (просто добавляем по два поля каждому).
    void paintCaptured(QPainter& painter)const;

    // заполянет один квадрат заданным цветом
    void drawQuadrat(QPainter& painter, QUADRAT const& quadrat, QColor const& color)const;
    void drawLine(QPainter& painter, LINE const& line, QColor const& color)const;
    void drawCross(QPainter& painter, QUADRAT const& quadrat, QColor const& color)const;
    void drawCircle(QPainter& painter, QUADRAT const& quadrat, QColor const& color)const;

    // Возврщает индекс квадрата в глобальной системе координат (относительно левого верхнего угла).
    QUADRAT getQuadrat(int x, int y)const;
    QUADRAT translateQuadrat(QUADRAT const& quadrat)const;

    // Возврщает линию квадрата в глобальной системе координат (относительно левого верхнего угла).
    LINE getLine(int x, int y)const;
    LINE translateLine(LINE const& line)const;

    // Проверяет находится ли квадрат внутри игрового поля.
    bool isInside(QUADRAT const& quadrat)const;
    bool isInside(LINE const& line)const;

    PLAYER_STATS m_stats[2];
    unsigned int m_dimFull; // размер игрового поля с учётом отступов
    unsigned int m_dim; // размер реального игрового поля
    int m_x;
    int m_y;

};

#endif
