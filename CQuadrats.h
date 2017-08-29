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

// структура хранит ходы и активные действия игроков
struct PLAYER_STATS
{
    QVector<QPoint> lines;
    QColor color;
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
    QSize m_dimension; // размер одного квадрата
    int m_x;
    int m_y;
    //unsigned int m_
    static QColor const sm_backgroundColor;
    static QColor const sm_lineColor;
    static QColor const sm_activeLineColor;
    /*static QColor const sm_sideLineColor;
    static QColor const sm_activeLineColor;*/
    virtual void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    //virtual void mouseReleaseEvent(QMouseEvent* event);
};

#endif
