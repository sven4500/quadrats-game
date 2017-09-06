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

struct LINE
{
    enum Orientation{Unknown = -1, Left = 0, Up, Right, Down};

    LINE(): orientation(Unknown)
    {
        index.x = -1;
        index.y = -1;
    }

    LINE(int x, int y, Orientation orientation): orientation(orientation)
    {
        index.x = x;
        index.y = y;
    }

    Orientation orientation; // какая сторона квадрата
    struct{
        int x:16;
        int y:16;
    }index; // какой квадрат
};

// структура хранит ходы игрока
struct PLAYER_STATS
{
    QVector<LINE> lines;
    QColor player;
};

class CQuadrats : public QMainWindow
{

    Q_OBJECT

public:

    CQuadrats(QWidget* parent = 0);
    ~CQuadrats();

private:

    /*enum{BACK_COL = 0, LINE_COL, ACTLINE_COL, P1LINE_COL, P2LINE_COL};
    QColor const sm_colorProfile[] = {
        QColor(250, 254, 255), // BACK_COL
        QColor()
    };*/
    //QSize m_dimension; // размер сетки
    unsigned int m_dim; // размер сетки
    int m_x;
    int m_y;
    //unsigned int m_

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

    LINE getLine(int x, int y)const;
};

#endif
