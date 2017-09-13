#include "quadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);
QColor const CQuadrats::sm_sideLineColor = QColor(243, 22, 72);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    m_playerOneColor = QColor(0, 162, 232);
    m_playerTwoColor = QColor(237, 28, 36);

    m_dimFull = 19; // сетка по-умолчанию
    m_dim = 9;

    m_stats[0].playerColor = m_playerOneColor;
    m_stats[1].playerColor = m_playerTwoColor;

    // создаём меню с параметрами в заголовке окна.
    {
        QMenuBar* const menu = menuBar();
        menu->addAction("Создать");
        menu->addAction("Присоединиться");
        //menu->addAction("Настройки");
        menu->addAction("Как играть");
        menu->addAction("О программе");
    }

    setMouseTracking(true);

    #ifdef _DEBUG
    setWindowTitle("Квадраты (версия 0.19 отладка)");
    #else
    setWindowTitle("Квадраты (версия 0.19)");
    #endif
}

CQuadrats::~CQuadrats()
{

}

unsigned int CQuadrats::getOneSize()const
{
    // игровое поле никогда не должно быть нулевым
    assert(m_dimFull != 0);
    return std::min(width(), height()) / m_dimFull;
}

CQuadrats::QUADRAT CQuadrats::getQuadrat(int x, int y)const
{
    unsigned int const oneSize = getOneSize();
    return QUADRAT(x / oneSize, y / oneSize);
}

CQuadrats::QUADRAT CQuadrats::translateQuadrat(QUADRAT const& quadrat)const
{
    QUADRAT const q1 = getQuadrat(width() / 2, height() / 2);
    QUADRAT q2 = quadrat;

    switch(quadrat.origin)
    {
    case QUADRAT::Origin::Local:
        q2.origin = QUADRAT::Origin::Global;
        q2.x += q1.x;
        q2.y += q1.y;
        break;
    case QUADRAT::Origin::Global:
        q2.origin = QUADRAT::Origin::Local;
        q2.x -= q1.x;
        q2.y -= q1.y;
        break;
    default:
        assert(false);
        break;
    }

    return q2;
}

CQuadrats::LINE CQuadrats::getLine(int x, int y)const
{
    unsigned int const oneSize = getOneSize();

    // расстояния от точки до четырёх границ квадрата
    unsigned int dist[4] = {};

    // ситаем расстояния до границ: левой, верхней, правой и нижней соответственно
    dist[0] = x % oneSize;
    dist[1] = y % oneSize;
    dist[2] = oneSize - dist[0];
    dist[3] = oneSize - dist[1];

    unsigned int orientation = 0;

    // ищем границу с наименьшим расстоянием до точки
    {
        unsigned int d = dist[0];
        orientation = 0;

        // находим минимальное расстояние
        for(unsigned int i = 1; i < 4; ++i)
        {
            if(dist[i] < d)
            {
                orientation = i;
                d = dist[i];
            }
        }
    }

    LINE line;

    line.quadrat = getQuadrat(x, y);

    switch(orientation)
    {
    case 0:
        line.orient = LINE::Left;
        break;
    case 1:
        line.orient = LINE::Up;
        break;
    case 2:
        line.orient = LINE::Right;
        break;
    case 3:
        line.orient = LINE::Down;
        break;
    default:
        // сюда не должны попадать в принципе
        assert(false);
        line.orient = LINE::Unknown;
        break;
    }

    return line;
}

CQuadrats::LINE CQuadrats::translateLine(LINE const& line)const
{
    LINE l = line;
    l.quadrat = translateQuadrat(l.quadrat);
    return l;
}

bool CQuadrats::isInside(QUADRAT const& quadrat)const
{
    // Если квадрат изначально в глобальных координатах, то сперва преобразуем в локальные.
    QUADRAT const q = (quadrat.isGlobal() == true) ? translateQuadrat(quadrat) : quadrat;
    unsigned int const dim2 = m_dim / 2;
    return std::abs(q.x) + std::abs(q.y) <= dim2;
}

void CQuadrats::mouseMoveEvent(QMouseEvent* event)
{
    // сохранили последние координаты указателя
    m_x = event->x();
    m_y = event->y();

    // перерисовываем рабочую область
    update();
}

// Здесь будет сосредоточена основная логика игры.
void CQuadrats::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    #ifdef _DEBUG
    QUADRAT quadrat;
    quadrat = getQuadrat(m_x, m_y);
    qDebug() << quadrat.x << quadrat.y << "\n";
    quadrat = translateQuadrat(quadrat);
    qDebug() << quadrat.x << quadrat.y << "\n\n";
//    LINE line;
//    line = getGlobalLine(m_x, m_y);
//    qDebug() << line.pos.x << line.pos.y << line.orient;
//    line = translateLine(line);
//    qDebug() << line.pos.x << line.pos.y << line.orient;
//    qDebug() << '\n';
    #endif
}

void CQuadrats::wheelEvent(QWheelEvent* event)
{
    assert(m_dimFull >= m_dim);

    QPoint const angle = event->angleDelta();

    if(angle.y() > 0)
    {
        if(m_dimFull > m_dim)
        {
            --m_dimFull;
        }
    }
    else if(angle.y() < 0)
    {
        if(m_dimFull < m_dim * 2)
        {
            ++m_dimFull;
        }
    }

    event->accept();
    update();
}
