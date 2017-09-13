#include "quadrats.h"

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // Инициализируем устройство рисования и очищем рабочу область перед рисовнием нового кадра.
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // Заполняет цветом квадраты на вершинах (нужно будет скоро избавиться и
    // просто добавлять каждому игроку изначально по два квадрата).
    painter.save();
    paintCorner(painter);
    painter.restore();

    // Подсвечиваем квадрат на который указывает указатель.
    painter.save();
    paintCurrentQuadrat(painter);
    painter.restore();

    // Рисуем сетку на всей рабочей области.
    painter.save();
    paintBackground(painter);
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

void CQuadrats::paintBackground(QPainter& painter)const
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

void CQuadrats::paintBorder(QPainter& painter)const
{
//    assert(m_dimFull > 0);

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

void CQuadrats::paintCurrentQuadrat(QPainter& painter)const
{
    QUADRAT const quadrat = getQuadrat(m_x, m_y);

    if(isInside(quadrat) == true)
    {
        fillQuadrat(painter, quadrat, QColor(190, 190, 190, 190));
//        markQuadrat(painter, quadrat, PlayerOne);
    }
}

void CQuadrats::paintCurrentLine(QPainter& painter)const
{
    // размер одного квадрата в пикселях
    unsigned int const oneSize = std::min(width(), height()) / m_dimFull;

    LINE const line = getLine(m_x, m_y);

    if(isInside(line.quadrat) == true)
    {
        {
            QPen pen;
            pen.setColor(sm_activeLineColor);
            pen.setWidth(2);

            painter.setPen(pen);
        }

        switch(line.orient)
        {
        case LINE::Left:
            painter.drawLine(line.quadrat.x * oneSize, line.quadrat.y * oneSize,
                line.quadrat.x * oneSize, (line.quadrat.y + 1) * oneSize);
            break;
        case LINE::Up:
            painter.drawLine(line.quadrat.x * oneSize, line.quadrat.y * oneSize,
                (line.quadrat.x + 1) * oneSize, line.quadrat.y * oneSize);
            break;
        case LINE::Right:
            painter.drawLine((line.quadrat.x + 1) * oneSize, line.quadrat.y * oneSize,
                (line.quadrat.x + 1) * oneSize, (line.quadrat.y + 1) * oneSize);
            break;
        case LINE::Down:
            painter.drawLine(line.quadrat.x * oneSize, (line.quadrat.y + 1) * oneSize,
                (line.quadrat.x + 1) * oneSize, (line.quadrat.y + 1) * oneSize);
            break;
        default:
            break;
        };
    }
}

void CQuadrats::paintCorner(QPainter& painter)const
{
    QUADRAT quadrats[4];

    {
        QUADRAT const quadrat = getQuadrat(width() / 2, height() / 2);
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

    fillQuadrat(painter, quadrats[0], m_playerOneColor);
    fillQuadrat(painter, quadrats[1], m_playerOneColor);
    fillQuadrat(painter, quadrats[2], m_playerTwoColor);
    fillQuadrat(painter, quadrats[3], m_playerTwoColor);
}

// Отрисовывает захваченные игроками квадраты.
void CQuadrats::paintCaptured(QPainter& painter)const
{
    Q_UNUSED(painter);
}

void CQuadrats::fillQuadrat(QPainter& painter, QUADRAT const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();
    painter.fillRect(quadrat.x * oneSize, quadrat.y * oneSize, oneSize, oneSize, color);
}

void CQuadrats::markQuadrat(QPainter& painter, QUADRAT const& quadrat, Player const& player)const
{
    unsigned int const oneSize = getOneSize();
    unsigned int const margin = oneSize * 0.25;
    unsigned int const margin2 = margin * 2;

    QRect const rect(quadrat.x * oneSize + margin, quadrat.y * oneSize + margin,
                     oneSize - margin2, oneSize - margin2);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    switch(player)
    {
        case Player::PlayerOne:
        {
            QPen pen;
            pen.setWidth(2);
            pen.setColor(m_playerOneColor);
            painter.setPen(pen);
            painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());
            painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.top());
            break;
        }
        case Player::PlayerTwo:
        {
            QPen pen;
            pen.setWidth(2);
            pen.setColor(m_playerTwoColor);
            painter.setPen(pen);
            painter.drawEllipse(rect);
            break;
        }
        default:
        {
            // Сюда вообще не должны попадать.
            assert(false);
            break;
        }
    }
}
