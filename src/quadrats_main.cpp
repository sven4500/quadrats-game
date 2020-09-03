#include <quadrats.h>
#include <version.h>

QColor const QuadratsGame::sm_backgroundColor = QColor(250, 254, 254);
QColor const QuadratsGame::sm_gridColor = QColor(172, 222, 254);

QuadratsGame::QuadratsGame(QWidget* parent): QMainWindow(parent)
{
    m_player = PlayerOne;

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

    // Формируем список методов для отрисовки в порядке добавления.
    m_painterFuncs.append(&paintBackgroud);
    m_painterFuncs.append(&paintCurrentQuadrat);
    m_painterFuncs.append(&paintGrid);
    m_painterFuncs.append(&paintPlayerLines);
    m_painterFuncs.append(&paintCurrentLine);
    m_painterFuncs.append(&paintGameBorder);

    setMouseTracking(true);

    QString const windowTitle = QString(
        #ifdef _DEBUG
        "Квадраты (сборка %1 отладка)"
        #else
        "Квадраты (сборка %1)"
        #endif
        ).arg(BuildVersion);

    setWindowTitle(windowTitle);

    // Создаём таймер который раз в 35 мс будет вызывать метод для обновления рабочей области окна.
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, static_cast<void (QWidget::*)()>(&QWidget::update));
    m_timer->start(35);
}

QuadratsGame::~QuadratsGame()
{
    //m_timer->stop();
}

unsigned int QuadratsGame::getOneSize()const
{
    // игровое поле никогда не должно быть нулевым
    assert(m_dimFull != 0);
    return std::min(width(), height()) / m_dimFull;
}

Quadrat QuadratsGame::getQuadrat(int x, int y)const
{
    unsigned int const oneSize = getOneSize();
    return Quadrat(x / oneSize, y / oneSize);
}

Quadrat QuadratsGame::translateQuadrat(Quadrat const& quadrat)const
{
    Quadrat const q1 = getQuadrat(width() / 2, height() / 2);
    Quadrat q2 = quadrat;

    switch(quadrat.origin)
    {
    case Quadrat::Origin::Local:
        q2.origin = Quadrat::Origin::Global;
        q2.x = q2.x + q1.x;
        q2.y = q1.y - q2.y;
        break;
    case Quadrat::Origin::Global:
        q2.origin = Quadrat::Origin::Local;
        q2.x = q2.x - q1.x;
        q2.y = q1.y - q2.y;
        break;
    default:
        assert(false);
        break;
    }

    return q2;
}

Line QuadratsGame::getLine(int x, int y)const
{
    Line line;
    line.origin = Line::Global;

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
        line.orientation = Line::Vertical;
        line.x = ix;
        line.y = iy;
        break;
    case 1:
        line.orientation = Line::Horizontal;
        line.x = ix;
        line.y = iy;
        break;
    case 2:
        line.orientation = Line::Vertical;
        line.x = ix + 1;
        line.y = iy;
        break;
    case 3:
        line.orientation = Line::Horizontal;
        line.x = ix;
        line.y = iy + 1;
        break;
    default:
        assert(false);
        break;
    }

    return line;
}

Line QuadratsGame::translateLine(Line const& inpLine)const
{
    Quadrat const relQuad = getQuadrat(width() / 2, height() / 2);
    Line outLine = inpLine;

    switch(outLine.origin)
    {
    case Line::Origin::Local:
        outLine.origin = Line::Origin::Global;
        outLine.x = outLine.x + relQuad.x;
        outLine.y = relQuad.y - outLine.y;
        break;

    case Line::Origin::Global:
        outLine.origin = Line::Origin::Local;
        outLine.x = outLine.x - relQuad.x;
        outLine.y = relQuad.y - outLine.y;
        break;

    default:
        assert(false);
        break;
    }

    return outLine;
}

bool QuadratsGame::isInside(Quadrat const& quadrat)const
{
    // Если квадрат изначально в глобальных координатах, то сперва преобразуем в локальные.
    Quadrat const q = (quadrat.isGlobal() == true) ? translateQuadrat(quadrat) : quadrat;
    unsigned int const dim2 = m_dim / 2;
    return (unsigned int)std::abs(q.x) + (unsigned int)std::abs(q.y) <= dim2;
}

bool QuadratsGame::isInside(Line const& line)const
{
    Line l = (line.isGlobal() == true) ? translateLine(line) : line;

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
        case Line::Orientation::Horizontal:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) < dim2;
        case Line::Orientation::Vertical:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) <= dim2;
        default:
            return false;
        }
    }
    else if(l.x <= 0 && l.y < 0)
    {
        switch(l.orientation)
        {
        case Line::Orientation::Horizontal:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) <= dim2;
        case Line::Orientation::Vertical:
            return (unsigned int)std::abs(l.x) + (unsigned int)std::abs(l.y) < dim2;
        default:
            return false;
        }
    }

    return false;
}

void QuadratsGame::mouseMoveEvent(QMouseEvent* event)
{
    // Сохраняем последние координаты указателя мыши.
    m_x = event->x();
    m_y = event->y();
}

// Здесь будет сосредоточена основная логика игры.
void QuadratsGame::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    #ifdef _DEBUG
    Line line;
    line = getLine(m_x, m_y);
    qDebug() << line.x << line.y << "\n";
    line = translateLine(line);
    qDebug() << line.x << line.y << "\n";
    line = translateLine(line);
    qDebug() << line.x << line.y << "\n\n";
    #endif
}

void QuadratsGame::mouseReleaseEvent(QMouseEvent* event)
{
    Line const line = getLine(event->x(), event->y());

    if(isInside(line) == true)
    {
        if(m_stats[m_player].contains(line) == false && m_stats[m_player].contains(line) == false)
        {
            m_stats[m_player].lines.append(translateLine(line));
        }
    }
}

void QuadratsGame::wheelEvent(QWheelEvent* event)
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
