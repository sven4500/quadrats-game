#include "quadrats.h"

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // инициализируем устройство рисования и очищем рабочу область перед рисовнием кадра
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // заполняет цветом вершинные квадраты
    paintCorner(painter);

    // подсвечиваем активный квадрат
    paintCurrentQuadrat(painter);

    // рисуем сетку в центре окна
    paintBackground(painter);

    // сисуем границы игрового поля
    paintBorder(painter);

    // рисуем текущую линию
    paintCurrentLine(painter);
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
    assert(m_dimFull > 0);

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

    // рисуем все горизонтальные линии
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
    fillQuadrat(painter, quadrat, QColor(190, 190, 190, 190));
}

void CQuadrats::paintCurrentLine(QPainter& painter)const
{
    // размер одного квадрата в пикселях
    unsigned int const oneSize = std::min(width(), height()) / m_dimFull;

    LINE const line = getLine(m_x, m_y);

    {
        QPen pen;
        pen.setColor(sm_activeLineColor);
        pen.setWidth(2);
        painter.setPen(pen);
    }

    switch(line.orient)
    {
    case LINE::Left:
        painter.drawLine(line.pos.x * oneSize, line.pos.y * oneSize,
            line.pos.x * oneSize, (line.pos.y + 1) * oneSize);
        break;
    case LINE::Up:
        painter.drawLine(line.pos.x * oneSize, line.pos.y * oneSize,
            (line.pos.x + 1) * oneSize, line.pos.y * oneSize);
        break;
    case LINE::Right:
        painter.drawLine((line.pos.x + 1) * oneSize, line.pos.y * oneSize,
            (line.pos.x + 1) * oneSize, (line.pos.y + 1) * oneSize);
        break;
    case LINE::Down:
        painter.drawLine(line.pos.x * oneSize, (line.pos.y + 1) * oneSize,
            (line.pos.x + 1) * oneSize, (line.pos.y + 1) * oneSize);
        break;
    default:
        break;
    };
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

void CQuadrats::fillQuadrat(QPainter& painter, QUADRAT const& quadrat, QColor const& color)const
{
    unsigned int const oneSize = getOneSize();
    painter.fillRect(quadrat.x * oneSize, quadrat.y * oneSize, oneSize, oneSize, color);
}
