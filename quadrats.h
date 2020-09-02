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
#include <QTimer>
#include <cassert>

class CQuadrats : public QMainWindow
{

    Q_OBJECT

public:

    CQuadrats(QWidget* parent = 0);
    ~CQuadrats();

private:

    enum PLAYER{PlayerOne = 0, PlayerTwo = 1};

    // Структура хранит информацию о местоположении одного квадрата.
    struct QUADRAT
    {
        // Система отсчёта. Относительно левого верхнего угла (Global), либо относительно центра игрового поля (Local).
        enum Origin{Local = 0, Global};

        QUADRAT():
            origin(Global), x(0), y(0)
        {

        }

        QUADRAT(int x, int y, Origin origin = Global):
            origin(origin), x(x), y(y)
        {

        }

        inline bool isLocal()const
        {
            return origin == Local;
        }

        inline bool isGlobal()const
        {
            return origin == Global;
        }

        inline bool operator==(QUADRAT const& other)const
        {
            return x == other.x && y == other.y;
        }

        Origin origin;  // Система отсчёта: локальная или глобальная.
        int x;          // Координата квадрата.
        int y;          // Координата квадрата.
    };

    // Структура хранит информацию о местоположении одной линии.
    struct LINE
    {
        enum Origin{Local = 0, Global};
        enum Orientation{Horizontal = 0, Vertical};

        LINE():
            origin(Global), orientation(Vertical), x(0), y(0)
        {

        }

        LINE(int x, int y, Orientation orientation, Origin origin = Global):
            origin(origin), orientation(orientation), x(x), y(y)
        {

        }

        inline bool isLocal()const
        {
            return origin == Local;
        }

        inline bool isGlobal()const
        {
            return origin == Global;
        }

        inline bool operator==(LINE const& other)const
        {
            return orientation == other.orientation && x == other.x && y == other.y;
        }

        Origin origin;              // Система координат: локальная или глобальная.
        Orientation orientation;    // Расположение линии: горизонтальное или вертикальное.
        int x;                      // Координата линии.
        int y;                      // Координата линии.
    };

    // структура хранит ходы одного игрока
    struct PLAYER_STATS
    {
        inline bool contains(LINE const& line)const
        {
            return lines.contains(line);
        }

        inline bool contains(QUADRAT const& quadrat)const
        {
            return quadrats.contains(quadrat);
        }

        QVector<LINE> lines;
        QVector<QUADRAT> quadrats;
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
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    unsigned int getOneSize()const; // возвращает рзмер одного квдрт в пикселях

    void paintBackground(QPainter& painter)const;
    void paintBorder(QPainter& painter)const;
    void paintCurrentQuadrat(QPainter& painter)const;
    void paintCurrentLine(QPainter& painter)const;
    void paintCorner(QPainter& painter)const; // <= исчезнет после paintCaptured (просто добавляем по два поля каждому).
    void paintPlayerLines(QPainter& painter)const;
    void paintPlayerQuadrats(QPainter& painter)const;
//    void paintCaptured(QPainter& painter)const;

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

    QTimer* m_timer;

    PLAYER_STATS m_stats[2];
    PLAYER m_player;
    unsigned int m_dimFull; // размер игрового поля с учётом отступов
    unsigned int m_dim; // размер реального игрового поля
    int m_x;
    int m_y;

};

#endif
