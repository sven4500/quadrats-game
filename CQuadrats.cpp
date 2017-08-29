#include "CQuadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    // создаём меню с параметрами в заголовке окна.
    {
        QMenuBar* const menu = menuBar();
        menu->addAction("Создать");
        menu->addAction("Присоединиться");
        menu->addAction("Настройки");
        menu->addAction("Помощь");
        menu->addAction("О программе");
    }

    m_dimension.setWidth(16);
    m_dimension.setHeight(16);
    setMouseTracking(true);
}

CQuadrats::~CQuadrats()
{
}

struct LINE
{
    enum Orientation{Unknown = -1, Left = 0, Up, Right, Down};

    LINE(): orientation(Unknown)
    {
        position.x = 0;
        position.y = 0;
    }

    /*LINE(int x, int y): orientation(Unknown)
    {
        position.x = x;
        position.y = y;
    }*/

    Orientation orientation; // какая сторона квадрата
    struct{
        int x:16;
        int y:16;
    }position; // какой квадрат
};

LINE getLine(int x, int y)
{
    return LINE();
}

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // рисуем сетку
    {
        unsigned int const horzLineCount = height() / m_dimension.height(),
            vertLineCount = width() / m_dimension.width();
        QVector<QPoint> grid;
        grid.reserve(horzLineCount * 2 + vertLineCount * 2);
        for(unsigned int i = 1; i <= horzLineCount; ++i)
        {
            unsigned int const pos = m_dimension.height() * i;
            grid.append(QPoint(0, pos));
            grid.append(QPoint(width(), pos));
        }
        for(unsigned int i = 1; i <= vertLineCount; ++i)
        {
            unsigned int const pos = m_dimension.width() * i;
            grid.append(QPoint(pos, 0));
            grid.append(QPoint(pos, height()));
        }
        painter.setPen(sm_lineColor);
        painter.drawLines(grid);
    }

    // рисуем линию на которую указывает указатель мышт
    {
        unsigned int const iX = m_x / m_dimension.width();
        unsigned int const iY = m_y / m_dimension.height();

        //unsigned int const modX = m_x % m_dimension.width();
        //unsigned int const modY = m_y % m_dimension.height();
        //unsigned int const modX2 = m_dimension.width() - modX;
        //unsigned int const modY2 = m_dimension.height() - modY;

        unsigned int index = 0;

        {
            QVector<int> dist(4); // расстояния от курсора до граней квадрата

            dist[0] = m_x % m_dimension.width();
            dist[1] = m_y % m_dimension.height();
            dist[2] = m_dimension.width() - dist[0];
            dist[3] = m_dimension.height() - dist[1];

            int m = dist[0];

            for(unsigned int i = 1; i < 4; ++i)
            {
                if(dist[i] < m)
                {
                    index = i;
                    m = dist[i];
                }
            }
        }

        QPen pen;
        pen.setColor(sm_activeLineColor);
        pen.setWidth(2);
        painter.setPen(pen);

        switch(index)
        {
        case 0:
            painter.drawLine(iX * m_dimension.width(), iY * m_dimension.height(),
                iX * m_dimension.width(), (iY + 1) * m_dimension.height());
            break;
        case 1:
            painter.drawLine(iX * m_dimension.width(), iY * m_dimension.height(),
                (iX + 1) * m_dimension.width(), iY * m_dimension.height());
            break;
        case 2:
            painter.drawLine((iX + 1) * m_dimension.width(), iY * m_dimension.height(),
                (iX + 1) * m_dimension.width(), (iY + 1) * m_dimension.height());
            break;
        case 3:
            painter.drawLine(iX * m_dimension.width(), (iY + 1) * m_dimension.height(),
                (iX + 1) * m_dimension.width(), (iY + 1) * m_dimension.height());
            break;
        default:
            break;
        };
    }
}

void CQuadrats::mouseMoveEvent(QMouseEvent* event)
{
    //Q_UNUSED(event);
    m_x = event->x();
    m_y = event->y();
    update(); // вызываем paintEvent
}

// Здесь будет сосредоточена основная логика игры.
void CQuadrats::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    if(false)
    {}
}
