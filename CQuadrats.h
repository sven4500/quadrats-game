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
            return orient != Unknown;
        }

        Orientation orient; // какая сторона квадрата
        struct{
            int x:16;
            int y:16;
        }pos; // какой квадрат
    };

    // структура хранит ходы игрока
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
    void paintCurrentLine(QPainter& painter);

    LINE getLine(int x, int y)const;
    LINE translateLine(LINE line)const;

    PLAYER_STATS m_stats[2];
    unsigned int m_dim; // размер сетки (m_dim x m_dim квадратов)
    int m_x;
    int m_y;

};

#endif
