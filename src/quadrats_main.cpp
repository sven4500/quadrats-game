#include <quadratsgame.h>
#include <version.h>

QColor const QuadratsGame::sm_backgroundColor = QColor(250, 254, 254);
QColor const QuadratsGame::sm_gridColor = QColor(172, 222, 254);

QuadratsGame::QuadratsGame(QWidget* parent): QMainWindow(parent)
{
    m_currentPlayer = PlayerOne;

    m_dimFull = 14;

    // Размер игрового пространства должен быть всегда меньше полного размера
    // пространства. Также игровое поле должно быть нечётным поэтому | 1.
    m_dim = m_dimFull * 0.75;
    m_dim |= 1;

    auto dimHalf = m_dim / 2;

    m_stats[0].playerColor = QColor(0, 162, 232);
    m_stats[0].quadrats.push_back(Quadrat(-dimHalf, 0, Quadrat::Local));
    m_stats[0].quadrats.push_back(Quadrat(+dimHalf, 0, Quadrat::Local));

    #ifdef _DEBUG
    m_stats[0].quadrats.push_back(Quadrat(-1, +1, Quadrat::Local));
    #endif

    m_stats[1].playerColor = QColor(237, 28, 36);
    m_stats[1].quadrats.push_back(Quadrat(0, -dimHalf, Quadrat::Local));
    m_stats[1].quadrats.push_back(Quadrat(0, +dimHalf, Quadrat::Local));

    #ifdef _DEBUG
    m_stats[1].quadrats.push_back(Quadrat(+1, -1, Quadrat::Local));
    #endif

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
    m_painterFuncs.append(&paintCapturedQuadrats);
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
    assert(m_dimFull > 0);
    int const size = std::min(width(), height());
    return size / m_dimFull;
}

Quadrat QuadratsGame::getQuadratGlobal(int x, int y)const
{
    unsigned int const oneSize = getOneSize();
    return Quadrat(x / oneSize, y / oneSize);
}

Quadrat QuadratsGame::toLocal(Quadrat const& quad)const
{
    if(quad.origin == Quadrat::Local)
        return quad;

    Quadrat const cent = getQuadratGlobal(width() / 2, height() / 2);
    Quadrat quadLocal;

    quadLocal.origin = Quadrat::Origin::Local;
    quadLocal.x = quad.x - cent.x;
    quadLocal.y = cent.y - quad.y;

    return quadLocal;
}

Quadrat QuadratsGame::toGlobal(Quadrat const& quad)const
{
    if(quad.origin == Quadrat::Global)
        return quad;

    Quadrat const cent = getQuadratGlobal(width() / 2, height() / 2);
    Quadrat quadGlobal;

    quadGlobal.origin = Quadrat::Origin::Global;
    quadGlobal.x = quad.x + cent.x;
    quadGlobal.y = cent.y - quad.y;

    return quadGlobal;
}

Line QuadratsGame::getLineGlobal(int x, int y)const
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

Line QuadratsGame::toLocal(Line const& line)const
{
    if(line.origin == Line::Local)
        return line;

    Quadrat const quad = getQuadratGlobal(width() / 2, height() / 2);

    Line lineLocal = line;

    lineLocal.origin = Line::Origin::Local;
    lineLocal.orientation = line.orientation;
    lineLocal.x = line.x - quad.x;
    lineLocal.y = quad.y - line.y;

    return lineLocal;
}

Line QuadratsGame::toGlobal(Line const& line)const
{
    if(line.origin == Line::Global)
        return line;

    Quadrat const quad = getQuadratGlobal(width() / 2, height() / 2);

    Line lineGlobal;

    lineGlobal.origin = Line::Origin::Global;
    lineGlobal.orientation = line.orientation;
    lineGlobal.x = line.x + quad.x;
    lineGlobal.y = quad.y - line.y;

    return lineGlobal;
}

bool QuadratsGame::isInside(Quadrat const& quadrat)const
{
    Quadrat const q = toLocal(quadrat);
    unsigned int const dim2 = m_dim / 2;
    return (unsigned int)std::abs(q.x) + (unsigned int)std::abs(q.y) <= dim2;
}

bool QuadratsGame::isInside(Line const& line)const
{
    Line l = toLocal(line);

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

bool QuadratsGame::isPlayerAcquired(Line const& line)const
{
    return m_stats[PlayerOne].contains(line) == true ||
        m_stats[PlayerTwo].contains(line) == true;
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
    {
        Line line;

        line = getLineGlobal(m_x, m_y);
        qDebug() << line.x << line.y;

        line = toLocal(line);
        qDebug() << line.x << line.y;
    }
    #endif
}

void QuadratsGame::mouseReleaseEvent(QMouseEvent* event)
{
    Line line = getLineGlobal(event->x(), event->y());

    if(isInside(line) == true)
    {
        // Линию получили в глобальной системе. Храним в относительной поэтому
        // сперва должны преобразовать.
        line = toLocal(line);

        if(isPlayerAcquired(line) == false)
        {
            m_stats[m_currentPlayer].lines.append(line);
            m_currentPlayer = (m_currentPlayer == PlayerOne) ? PlayerTwo : PlayerOne;
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
