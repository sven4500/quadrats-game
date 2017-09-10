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

    struct QUADRAT
    {
        QUADRAT(): x(0), y(0)
        {}

        QUADRAT(int x, int y): x(x), y(y)
        {}

        bool isValid()const
        {
            return true;
        }

        int x:16;
        int y:16;
    };

    // структура хранит информацию об отдельной линии
    struct LINE
    {
        enum Orientation{Unknown = -1, Left = 0, Up, Right, Down};

        LINE(): orient(Unknown)
        {
            pos.x = -1;
            pos.y = -1;
        }

        LINE(int x, int y, Orientation orientation): orient(orientation)
        {
            pos.x = x;
            pos.y = y;
        }

        bool isValid()const
        {
            return orient != Unknown && pos.isValid() == true;
        }

        Orientation orient; // какая сторона квадрата
        QUADRAT pos; // какой квадрат
    };

    // структура хранит ходы одного игрока
    struct PLAYER_STATS
    {
        QVector<LINE> lines;
        QColor player;
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

    //static QColor const sm_playerOneColor;
    //static QColor const sm_playerTwoColor;
    //static QColor const sm_activeLineColor;

    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    //virtual void mouseReleaseEvent(QMouseEvent* event);

    inline unsigned int getOneSize()const; // возвращает рзмер одного квдрт в пикселях

    void paintBackground(QPainter& painter);
    void paintBorder(QPainter& painter);
    void paintCurrentQuadrat(QPainter& painter);
    void paintCurrentLine(QPainter& painter);

    // заполянет один квадрат заданным цветом
    void fillQuadrat(QPainter& painter, QUADRAT const& quadrat, QColor const& color);

    // возврщает индекс квадрата в глобальной системе координат (относительно левого верхнего угла)
    QUADRAT getQuadrat(int x, int y)const;
//    QUADRAT translateQuadrat(QUADRAT const& quadrat)const;

    // возврщает линию квадрата в глобальной системе координат (относительно левого верхнего угла)
    LINE getLine(int x, int y)const;
//    LINE translateLine(LINE line)const;

    PLAYER_STATS m_stats[2];
    unsigned int m_dimFull; // размер игрового поля с учётом отступов
    unsigned int m_dim; // размер реального игрового поля
    int m_x;
    int m_y;

};

#endif
