#include <quadratsgame.h>
#include <transform.h>
#include <version.h>

QColor const QuadratsGame::sm_backgroundColor = QColor(250, 254, 254);
QColor const QuadratsGame::sm_gridColor = QColor(172, 222, 254);

QuadratsGame::QuadratsGame(QWidget* parent):
    QMainWindow(parent), m_timer(this)
{
    // Размер игрового пространства должен быть всегда меньше полного размера
    // пространства. Также игровое поле должно быть нечётным поэтому | 1.
    m_dimFull = 14;
    m_dim = m_dimFull * 0.75;
    m_dim |= 1;

    m_logic.resetState(m_dim);

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

Quadrat QuadratsGame::getQuadratCentral()const
{
    return getQuadratGlobal(width() / 2, height() / 2);
}

Quadrat QuadratsGame::getQuadratGlobal(int x, int y)const
{
    return Transform::getQuadratGlobal(x, y, getOneSize());
}

Quadrat QuadratsGame::toLocal(Quadrat const& quad)const
{
    return Transform::toLocal(quad, getQuadratCentral());
}

Quadrat QuadratsGame::toGlobal(Quadrat const& quad)const
{
    return Transform::toGlobal(quad, getQuadratCentral());
}

Line QuadratsGame::getLineGlobal(int x, int y)const
{
    return Transform::getLineGlobal(x, y, getOneSize());
}

Line QuadratsGame::toLocal(Line const& line)const
{
    return Transform::toLocal(line, getQuadratCentral());
}

Line QuadratsGame::toGlobal(Line const& line)const
{
    return Transform::toGlobal(line, getQuadratCentral());
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
}

void QuadratsGame::mouseReleaseEvent(QMouseEvent* event)
{
    Line line = getLineGlobal(event->x(), event->y());

    // Линию получили в глобальной системе. Храним в относительной поэтому
    // сперва должны преобразовать.
    line = toLocal(line);

    m_logic.addLine(line);
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
    auto const& quadOne = m_logic.quadrats(PlayerEnum::PlayerOne);

    for(int i = 0; i < quadOne.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadOne[i], PlayerEnum::PlayerOne);
    }

    auto const& quadTwo = m_logic.quadrats(PlayerEnum::PlayerTwo);

    for(int i = 0; i < quadTwo.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadTwo[i], PlayerEnum::PlayerTwo);
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
    Quadrat const global = getQuadratGlobal(m_x, m_y);
    Quadrat const local = toLocal(global);

    if(m_logic.isInside(local) == true)
    {
        drawQuadrat(painter, global, QColor(190, 190, 190, 190));
    }
}

void QuadratsGame::paintCurrentLine(QPainter& painter)const
{
    Line const line = toLocal(getLineGlobal(m_x, m_y));

    PlayerEnum const currentPlayer = m_logic.currentPlayer(),
        otherPlayer = m_logic.otherPlayer();

    if(m_logic.lines(otherPlayer).contains(line) == false && m_logic.isInside(line) == true)
    {
        drawLine(painter, line, m_logic.playerColor(currentPlayer));
    }
}

void QuadratsGame::paintPlayerLines(QPainter& painter)const
{
    auto const& linesP1 = m_logic.lines(PlayerEnum::P1);
    auto const& colorP1 = m_logic.playerColor(PlayerEnum::P1);

    for(int j = 0; j < linesP1.size(); ++j)
    {
        drawLine(painter, linesP1[j], colorP1);
    }

    auto const& linesP2 = m_logic.lines(PlayerEnum::P2);
    auto const& colorP2 = m_logic.playerColor(PlayerEnum::P2);

    for(int j = 0; j < linesP2.size(); ++j)
    {
        drawLine(painter, linesP2[j], colorP2);
    }
}

void QuadratsGame::paintScore(QPainter& painter)const
{
    painter.setFont(QFont("Courier New", 16, QFont::Normal));

    drawPlayerScore(painter, PlayerEnum::P1);
    drawPlayerScore(painter, PlayerEnum::P2);
}

void QuadratsGame::drawPlayerScore(QPainter& painter, PlayerEnum player)const
{
    QPoint const corner(30, 30);

    // Используем статические переменные которые будут хранить необходимый
    // размер для отрисовки всего текста на экране. Это значение получаем после
    // вызова метода drawText поэтому первый кадр будет не совсем удачный (но
    // это не заметно).
    static QRect br1(corner, QSize());
    static QRect br2(corner, QSize());

    QRect* drawRect = 0;

    int flags = Qt::AlignTop;

    if(player == PlayerEnum::PlayerOne)
    {
        drawRect = &br1;

        flags |= Qt::AlignLeft;
    }
    else
    {
        drawRect = &br2;

        drawRect->setLeft(width() - drawRect->width() - corner.x());
        drawRect->setRight(width() - corner.x());

        flags |= Qt::AlignRight;
    }

    assert(drawRect != nullptr);

    QString const scoreText = QString("%1 | %2")
        .arg(m_logic.quadrats(player).size())
        .arg(m_logic.lines(player).size());

    QRect const marginRect = drawRect->adjusted(-5, -5, 5, 5);

    painter.setPen(QPen(m_logic.playerColor(player), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));

    painter.drawRect(marginRect);
    painter.drawText(*drawRect, flags, scoreText, drawRect);
}

void QuadratsGame::drawCapturedQuadrat(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    QColor const color = m_logic.playerColor(player);
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
    if(player == PlayerEnum::PlayerOne)
        drawCross(painter, quad, m_logic.playerColor(player));
    else
    if(player == PlayerEnum::PlayerTwo)
        drawCircle(painter, quad, m_logic.playerColor(player));
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
