#include "quadrats.h"

void QuadratsGame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // Инициализируем устройство рисования и очищем рабочу область перед рисовнием нового кадра.
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // Заполняет цветом квадраты на вершинах (нужно будет скоро избавиться и
    // просто добавлять каждому игроку изначально по два квадрата).
//    painter.save();
//    paintCorner(painter);
//    painter.restore();

    // Подсвечиваем квадрат на который указывает указатель.
    painter.save();
    paintCurrentQuadrat(painter);
    painter.restore();

    // Рисуем сетку на всей рабочей области.
    painter.save();
    paintBackground(painter);
    painter.restore();

    // Рисуем линии обоих игроков.
    painter.save();
    paintPlayerLines(painter);
    painter.restore();

    // Рисуем текущую линию.
    painter.save();
    paintCurrentLine(painter);
    painter.restore();

    // Выделяем границы игрового поля.
    painter.save();
    paintBorder(painter);
    painter.restore();
}

void QuadratsGame::paintBackground(QPainter& painter)const
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
    painter.setPen(sm_lineColor);
    painter.drawLines(grid);

    // рисуем красную линию
//    painter.setPen(sm_sideLineColor);
//    painter.drawLine(oneSize * 4.5, 0, oneSize * 4.5, height());
}

//void CQuadrats::paintCaptured(QPainter& painter, int x, int y, )
//{}

void QuadratsGame::paintBorder(QPainter& painter)const
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
        drawLine(painter, line, sm_activeLineColor);
    }
}

void QuadratsGame::paintCorner(QPainter& painter)const
{
    Quadrat quadrats[4];

    {
        Quadrat const quadrat = getQuadrat(width() / 2, height() / 2);
        unsigned int const dim2 = m_dim / 2;

        quadrats[0] = quadrat;
        quadrats[0].x -= dim2;

        quadrats[1] = quadrat;
        quadrats[1].x += dim2;

        quadrats[2] = quadrat;
        quadrats[2].y -= dim2;

        quadrats[3] = quadrat;
        quadrats[3].y += dim2;
    }

    drawQuadrat(painter, quadrats[0], m_playerOneColor);
    drawQuadrat(painter, quadrats[1], m_playerOneColor);
    drawQuadrat(painter, quadrats[2], m_playerTwoColor);
    drawQuadrat(painter, quadrats[3], m_playerTwoColor);
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

void QuadratsGame::paintPlayerQuadrats(QPainter& painter)const
{
    Q_UNUSED(painter);
}

void QuadratsGame::drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color)const
{
    // Если квдрат в локальных координатах тогда сперва преобразуем в глобальные.
    Quadrat const q = (quadrat.isLocal() == true) ? translateQuadrat(quadrat) : quadrat;

    unsigned int const oneSize = getOneSize();

    painter.fillRect(q.x * oneSize, q.y * oneSize, oneSize, oneSize, color);
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

    // Вычисляем квадрат внутри основного квадрата где можно рисовать.
    QRect const rect(quadrat.x * oneSize + margin + 1, quadrat.y * oneSize + margin + 1, size, size);

    // Устанавливаем стиль отрисовки.
    {
        QPen pen;
        pen.setWidth(2);
        pen.setColor(color);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    }

    painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());
    painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.top());
}

void QuadratsGame::drawCircle(QPainter& painter, Quadrat const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();

    unsigned int const margin = oneSize * 0.25;
    unsigned int const size = oneSize - margin * 2;

    // Вычисляем квадрат внутри основного квадрата где можно рисовать.
    QRect const rect(quadrat.x * oneSize + margin + 1, quadrat.y * oneSize + margin + 1, size - 1, size - 1);

    // Устанавливаем стиль отрисовки.
    {
        QPen pen;
        pen.setWidth(2);
        pen.setColor(color);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    }

    painter.drawEllipse(rect);
}
