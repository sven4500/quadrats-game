#include "quadrats.h"

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
    Quadrat const quadrat = getQuadrat(m_x, m_y);

    if(isInside(quadrat) == true)
    {
        drawQuadrat(painter, quadrat, QColor(190, 190, 190, 190));
    }
}

void QuadratsGame::paintCurrentLine(QPainter& painter)const
{
    Line const line = getLine(m_x, m_y);

    if(isInside(line) == true)
    {
        drawLine(painter, line, m_playerOneColor);
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

void QuadratsGame::drawCapturedQuadrat(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    if(player == PlayerOne)
    {
        QColor const col = m_playerOneColor;
        drawQuadrat(painter, quad, col, Qt::Dense6Pattern);
        drawCross(painter, quad, col);
        drawQuadratBorders(painter, quad, col);
    }
    else
    {
        QColor const col = m_playerTwoColor;
        drawQuadrat(painter, quad, col, Qt::Dense6Pattern);
        drawCircle(painter, quad, col);
        drawQuadratBorders(painter, quad, col);
    }
}

void QuadratsGame::drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color, Qt::BrushStyle style)const
{
    // Если квдрат в локальных координатах тогда сперва преобразуем в глобальные.
    Quadrat const quad = quadrat.isLocal() ? translateQuadrat(quadrat) : quadrat;
    unsigned int const oneSize = getOneSize();

    QBrush brush;
    brush.setColor(color);
    brush.setStyle(style);

    painter.fillRect(quad.x * oneSize, quad.y * oneSize, oneSize, oneSize, brush);
}

void QuadratsGame::drawQuadratBorders(QPainter& painter, Quadrat const& quad, QColor const& color)const
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(Qt::SolidLine);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    Line horzLine(quad, Line::Horizontal);
    Line vertLine(quad, Line::Vertical);

    drawLine(painter, horzLine, color);
    horzLine.y -= 1;
    drawLine(painter, horzLine, color);

    drawLine(painter, vertLine, color);
    vertLine.x += 1;
    drawLine(painter, vertLine, color);
}

void QuadratsGame::drawLine(QPainter& painter, Line const& line, QColor const& color)const
{
    //qDebug() << line.x << line.y << "\n";

    // Если линия в локальных координатах то преобразуем в глобальные.
    Line const l = (line.isLocal() == true) ? translateLine(line) : line;

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

void QuadratsGame::drawCross(QPainter& painter, Quadrat const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();

    unsigned int const margin = oneSize * 0.25;
    unsigned int const size = oneSize - margin * 2;

    Quadrat const translated = translateQuadrat(quadrat);

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

    Quadrat const translated = translateQuadrat(quadrat);

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
