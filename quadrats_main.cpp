#include "quadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);
QColor const CQuadrats::sm_sideLineColor = QColor(243, 22, 72);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    m_playerOneColor = QColor(0, 162, 232);
    m_playerTwoColor = QColor(237, 28, 36);

    m_dimFull = 14; // сетка по-умолчанию
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

    // Создаём таймер который раз в 35 мс будет вызывать метод для обновления рабочей области окна.
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, static_cast<void (QWidget::*)()>(&QWidget::update));
    m_timer->start(35);
}

CQuadrats::~CQuadrats()
{
    m_timer->stop();
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
        q2.x = q2.x + q1.x;
        q2.y = q1.y - q2.y;
        break;
    case QUADRAT::Origin::Global:
        q2.origin = QUADRAT::Origin::Local;
        q2.x = q2.x - q1.x;
        q2.y = q1.y - q2.y;
        break;
    default:
        assert(false);
        break;
    }

    return q2;
}

CQuadrats::LINE CQuadrats::getLine(int x, int y)const
{
    LINE line;
    line.origin = LINE::Global;

    unsigned int const oneSize = getOneSize();
    unsigned int const ix = x / oneSize;
    unsigned int const iy = y / oneSize;

    // Расстояния от точки до четырёх границ квадрата: левой, верхней, правой и нижней соответственно.
    unsigned int const dist[4] = {
        x % oneSize,
        y % oneSize,
        oneSize - dist[0],
        oneSize - dist[1]
    };

    unsigned int j = 0;

    // Ищем границу с наименьшим расстоянием до точки.
    for(unsigned int i = 1; i < 4; ++i)
    {
        if(dist[i] < dist[j])
        {
            j = i;
        }
    }

    switch(j)
    {
    case 0:
        line.orientation = LINE::Vertical;
        line.x = ix;
        line.y = iy;
        break;
    case 1:
        line.orientation = LINE::Horizontal;
        line.x = ix;
        line.y = iy;
        break;
    case 2:
        line.orientation = LINE::Vertical;
        line.x = ix + 1;
        line.y = iy;
        break;
    case 3:
        line.orientation = LINE::Horizontal;
        line.x = ix;
        line.y = iy + 1;
        break;
    default:
        assert(false);
        break;
    }

    return line;
}

CQuadrats::LINE CQuadrats::translateLine(LINE const& line)const
{
    LINE const l1 = getLine(width() / 2, height() / 2);
    LINE l2 = line;

    switch(l2.origin)
    {
    case LINE::Origin::Local:
        l2.origin = LINE::Origin::Global;
        l2.x = l2.x + l1.x;
        l2.y = l1.y - l2.y;
        break;
    case LINE::Origin::Global:
        l2.origin = LINE::Origin::Local;
        l2.x = l2.x - l1.x;
        l2.y = l1.y - l2.y;
        break;
    default:
        assert(false);
        break;
    }

    return l2;
}

bool CQuadrats::isInside(QUADRAT const& quadrat)const
{
    // Если квадрат изначально в глобальных координатах, то сперва преобразуем в локальные.
    QUADRAT const q = (quadrat.isGlobal() == true) ? translateQuadrat(quadrat) : quadrat;
    unsigned int const dim2 = m_dim / 2;
    return (unsigned int)std::abs(q.x) + (unsigned int)std::abs(q.y) <= dim2;
}

bool CQuadrats::isInside(LINE const& line)const
{
    LINE l = (line.isGlobal() == true) ? translateLine(line) : line;

    unsigned int dim2 = m_dim / 2;

    if(l.x <= 0 && l.y >= 0)
    {
        return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) < dim2;
    }
    else if(l.x > 0 && l.y < 0)
    {
        return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) <= dim2;
    }
    else if(l.x > 0 && l.y >= 0)
    {
        switch(l.orientation)
        {
        case LINE::Orientation::Horizontal:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) < dim2;
        case LINE::Orientation::Vertical:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) <= dim2;
        default:
            return false;
        }
    }
    else if(l.x <= 0 && l.y < 0)
    {
        switch(l.orientation)
        {
        case LINE::Orientation::Horizontal:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) <= dim2;
        case LINE::Orientation::Vertical:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) < dim2;
        default:
            return false;
        }
    }

    return false;
}

void CQuadrats::mouseMoveEvent(QMouseEvent* event)
{
    // Сохраняем последние координаты указателя мыши.
    m_x = event->x();
    m_y = event->y();
}

// Здесь будет сосредоточена основная логика игры.
void CQuadrats::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    #ifdef _DEBUG
    LINE line;
    line = getLine(m_x, m_y);
    qDebug() << line.x << line.y << "\n";
    line = translateLine(line);
    qDebug() << line.x << line.y << "\n";
    line = translateLine(line);
    qDebug() << line.x << line.y << "\n\n";
    #endif
}

void CQuadrats::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void CQuadrats::wheelEvent(QWheelEvent* event)
{
    assert(m_dimFull >= m_dim);
    assert(m_dimFull % 2 == 0);
    assert(m_dim % 2 == 1);

    QPoint const angle = event->angleDelta();

    if(angle.y() > 0)
    {
        if(m_dimFull > m_dim + 1)
        {
            m_dimFull -= 2;
        }
    }
    else if(angle.y() < 0)
    {
        if(m_dimFull < m_dim * 2)
        {
            m_dimFull += 2;
        }
    }

    event->accept();
}
