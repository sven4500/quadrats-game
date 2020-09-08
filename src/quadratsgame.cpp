#include <quadratsgame.h>
#include <transform.h>
#include <version.h>

QColor const QuadratsGame::sm_backgroundColor = QColor(250, 254, 254);
QColor const QuadratsGame::sm_gridColor = QColor(172, 222, 254);

QuadratsGame::QuadratsGame(QWidget* parent):
    QMainWindow(parent), m_timer(this), m_currentPlayer(PlayerOne)
{
    // Размер игрового пространства должен быть всегда меньше полного размера
    // пространства. Также игровое поле должно быть нечётным поэтому | 1.
    m_dimFull = 14;
    m_dim = m_dimFull * 0.75;
    m_dim |= 1;

    addInitialStats();
    addPaintRoutines();

    //QMenuBar* const menu = menuBar();
    //menu->addAction("Создать");
    //menu->addAction("Присоединиться");
    //menu->addAction("Настройки");
    //menu->addAction("Как играть");
    //menu->addAction("О программе");

    setMouseTracking(true);

    QString const windowTitle = QString(
        #ifdef _DEBUG
        "Квадраты (сборка %1 отладка)"
        #else
        "Квадраты (сборка %1)"
        #endif
        ).arg(BuildVersion);

    setWindowTitle(windowTitle);

    connect(&m_timer, &QTimer::timeout, this, static_cast<void (QWidget::*)()>(&QWidget::update));
    m_timer.setInterval(50);
    m_timer.start();
}

QuadratsGame::~QuadratsGame()
{
    m_timer.stop();
}

void QuadratsGame::addInitialStats()
{
    auto dimHalf = m_dim / 2;

    // В начале игры оба игрока имеют по два захваченных квадрата на вершинах.
    Quadrat const leftQuad{-dimHalf, 0, Quadrat::Local},
        rightQuad{dimHalf, 0, Quadrat::Local},
        upperQuad{0, -dimHalf, Quadrat::Local},
        lowerQuad{0, dimHalf, Quadrat::Local};

    Line const leftLine = Transform::toLine(leftQuad, Line::Left),
        rightLine = Transform::toLine(rightQuad, Line::Right),
        upperLine = Transform::toLine(upperQuad, Line::Up),
        lowerLine = Transform::toLine(lowerQuad, Line::Down);

    m_stats[P1].playerColor = QColor(0, 162, 232);
    m_stats[P1].quadrats.push_back(upperQuad);
    m_stats[P1].quadrats.push_back(lowerQuad);
    m_stats[P1].lines.push_back(upperLine);
    m_stats[P1].lines.push_back(lowerLine);

    m_stats[P2].playerColor = QColor(237, 28, 36);
    m_stats[P2].quadrats.push_back(leftQuad);
    m_stats[P2].quadrats.push_back(rightQuad);
    m_stats[P2].lines.push_back(leftLine);
    m_stats[P2].lines.push_back(rightLine);
}

void QuadratsGame::addPaintRoutines()
{
    // Формируем список методов для отрисовки в порядке добавления.
    m_painterFuncs.append(&paintBackgroud);
    m_painterFuncs.append(&paintCurrentQuadrat);
    m_painterFuncs.append(&paintGrid);
    m_painterFuncs.append(&paintCapturedQuadrats);
    m_painterFuncs.append(&paintPlayerLines);
    m_painterFuncs.append(&paintCurrentLine);
    m_painterFuncs.append(&paintGameBorder);
    m_painterFuncs.append(&paintScore);
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

bool QuadratsGame::tryToEnclose(Line const& line)
{
    Q_UNUSED(line);
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

            // Попадаем внутрь функции которая проверяет закрывает добавленная
            // линия квадрат или нет.
            if(tryToEnclose(line) == false)
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

void QuadratsGame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    for(int i = 0; i < m_painterFuncs.size(); ++i)
    {
        PainterFunc const& painterFunc = m_painterFuncs[i];
        painter.save();
        (this->*painterFunc)(painter);
        painter.restore();
    }
}

void QuadratsGame::paintBackgroud(QPainter& painter)const
{
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);
}

void QuadratsGame::paintGrid(QPainter& painter)const
{
    // размер одного квадрата в пикселях
    unsigned int const oneSize = getOneSize();

    // сюда будем сохранять пары точек начала и конца каждой линии сетки
    QVector<QPoint> grid;

    {
        // количество горизонтальных и вертикальных линий
        unsigned int const horzLineCount = height() / oneSize;
        unsigned int const vertLineCount = width() / oneSize;

        grid.reserve(horzLineCount * 2 + vertLineCount * 2);

        for(unsigned int i = 1; i <= horzLineCount; ++i)
        {
            unsigned int const pos = oneSize * i;
            grid.append(QPoint(0, pos));
            grid.append(QPoint(width(), pos));
        }

        for(unsigned int i = 1; i <= vertLineCount; ++i)
        {
            unsigned int const pos = oneSize * i;
            grid.append(QPoint(pos, 0));
            grid.append(QPoint(pos, height()));
        }
    }

    // рисуем просчмианную сетку
    painter.setPen(sm_gridColor);
    painter.drawLines(grid);

    // рисуем красную линию
//    painter.setPen(sm_sideLineColor);
//    painter.drawLine(oneSize * 4.5, 0, oneSize * 4.5, height());
}

void QuadratsGame::paintCapturedQuadrats(QPainter& painter)const
{
    auto const& quadOne = m_stats[0].quadrats;

    for(int i = 0; i < quadOne.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadOne[i], PlayerOne);
    }

    auto const& quadTwo = m_stats[1].quadrats;

    for(int i = 0; i < quadTwo.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadTwo[i], PlayerTwo);
    }
}

void QuadratsGame::paintGameBorder(QPainter& painter)const
{
//    assert(m_dimFull >= m_dim && m_dim != 0);

    // размер одного квадрата в пикселях
    unsigned int const oneSize = getOneSize();

    // количество горизонтальных и вертикальных линий
    unsigned int const horzLineCount = height() / oneSize;
    unsigned int const vertLineCount = width() / oneSize;

    // находим центр игрового поля
    unsigned int const ix = vertLineCount / 2;
    unsigned int const iy = horzLineCount / 2;

    unsigned int const dim2 = m_dim / 2;

    // устанавливаем свойства пера границ игрового поля
    {
        QPen pen;
        pen.setColor(QColor(64, 64, 64));
        pen.setWidth(2);

        painter.setPen(pen);
    }

    for(unsigned int i = 0; i <= dim2; ++i)
    {
        unsigned int const x[4] = {
            (ix - i) * oneSize,
            (ix + i + 1) * oneSize,
            x[0] + oneSize,
            x[1] - oneSize
        };

        unsigned int const y[4] = {
            (iy - dim2 + i) * oneSize,
            (iy + dim2 - i + 1) * oneSize,
            y[0] + oneSize,
            y[1] - oneSize
        };

        painter.drawLine(x[0], y[0], x[2], y[0]);
        painter.drawLine(x[0], y[0], x[0], y[2]);

        painter.drawLine(x[1], y[0], x[3], y[0]);
        painter.drawLine(x[1], y[0], x[1], y[2]);

        painter.drawLine(x[0], y[1], x[2], y[1]);
        painter.drawLine(x[0], y[1], x[0], y[3]);

        painter.drawLine(x[1], y[1], x[3], y[1]);
        painter.drawLine(x[1], y[1], x[1], y[3]);
    }
}

void QuadratsGame::paintCurrentQuadrat(QPainter& painter)const
{
    Quadrat const quadrat = getQuadratGlobal(m_x, m_y);

    if(isInside(quadrat) == true)
    {
        drawQuadrat(painter, quadrat, QColor(190, 190, 190, 190));
    }
}

void QuadratsGame::paintCurrentLine(QPainter& painter)const
{
    Line const line = getLineGlobal(m_x, m_y);

    if(isInside(line) == true)
    {
        drawLine(painter, line, m_stats[m_currentPlayer].playerColor);
    }
}

void QuadratsGame::paintPlayerLines(QPainter& painter)const
{
    for(int i = 0; i < 2; ++i)
    {
        for(int j = 0; j < m_stats[i].lines.size(); ++j)
        {
            drawLine(painter, m_stats[i].lines[j], m_stats[i].playerColor);
        }
    }
}

void QuadratsGame::paintScore(QPainter& painter)const
{
    painter.setFont(QFont("Courier New", 16, QFont::Bold));

    drawPlayerScore(painter, PlayerOne);
    drawPlayerScore(painter, PlayerTwo);
}

void QuadratsGame::drawPlayerScore(QPainter& painter, PlayerEnum player)const
{
    QRect drawRect;
    int flags = Qt::AlignTop;

    if(player == PlayerOne)
    {
        drawRect = QRect(20, 50, 250, 100);
        flags |= Qt::AlignLeft;
    }
    else
    {
        drawRect = QRect(width() - 250 - 20, 50, 250, 100);
        flags |= Qt::AlignRight;
    }

    QString const scoreText = QString("%1 | %2")
        .arg(m_stats[player].lines.size())
        .arg(m_stats[player].quadrats.size());

    painter.setPen(QPen(m_stats[player].playerColor, 1));
    painter.drawText(drawRect, flags, scoreText, Q_NULLPTR);
}

void QuadratsGame::drawCapturedQuadrat(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    QColor const color = m_stats[player].playerColor;
    drawQuadrat(painter, quad, color, Qt::Dense6Pattern);
    drawPlayerInsignia(painter, quad, player);
}

void QuadratsGame::drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color, Qt::BrushStyle style)const
{
    Quadrat const quad = toGlobal(quadrat);
    unsigned int const oneSize = getOneSize();

    QBrush brush;
    brush.setColor(color);
    brush.setStyle(style);

    painter.fillRect(quad.x * oneSize, quad.y * oneSize, oneSize, oneSize, brush);
}

void QuadratsGame::drawLine(QPainter& painter, Line const& line, QColor const& color)const
{
    //qDebug() << line.x << line.y << "\n";

    Line const l = toGlobal(line);

    //qDebug() << l.x << l.y << "\n\n";

    unsigned int const oneSize = getOneSize();
    unsigned int const ix = l.x * oneSize;
    unsigned int const iy = l.y * oneSize;

    // Устанавливаем стиль рисования.
    {
        QPen pen;
        pen.setColor(color);
        pen.setWidth(2);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
    }

    switch(l.orientation)
    {
    case Line::Horizontal:
        painter.drawLine(ix, iy, ix + oneSize, iy);
        break;

    case Line::Vertical:
        painter.drawLine(ix, iy, ix, iy + oneSize);
        break;

    default:
        assert(false);
        break;
    };
}

void QuadratsGame::drawPlayerInsignia(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    if(player == PlayerOne)
        drawCross(painter, quad, m_stats[player].playerColor);
    else
    if(player == PlayerTwo)
        drawCircle(painter, quad, m_stats[player].playerColor);
    else
        assert(false);
}

void QuadratsGame::drawCross(QPainter& painter, Quadrat const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();

    unsigned int const margin = oneSize * 0.25;
    unsigned int const size = oneSize - margin * 2;

    Quadrat const translated = toGlobal(quadrat);

    QRect const rect(translated.x * oneSize + margin + 1, translated.y * oneSize + margin + 1, size, size);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(color);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());
    painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.top());
}

void QuadratsGame::drawCircle(QPainter& painter, Quadrat const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();

    unsigned int const margin = oneSize * 0.25;
    unsigned int const size = oneSize - margin * 2;

    Quadrat const translated = toGlobal(quadrat);

    QRect const rect(translated.x * oneSize + margin + 1, translated.y * oneSize + margin + 1, size - 1, size - 1);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(color);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    painter.drawEllipse(rect);
}
