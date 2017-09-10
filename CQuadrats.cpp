#include "CQuadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);
QColor const CQuadrats::sm_sideLineColor = QColor(243, 22, 72);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    m_dimFull = 13; // сетка по-умолчанию
    m_dim = 9;

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
    setWindowTitle("Квадраты (версия 0.15 отладка)");
    #else
    setWindowTitle("Квадраты (версия 0.15)");
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

    line.pos = getQuadrat(x, y);

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

//CQuadrats::LINE CQuadrats::translateLine(LINE line)const
//{
//    if(line.isValid() == true)
//    {
//        unsigned int const oneSize = getOneSize();
//        unsigned int const horzLineCount = height() / oneSize;
//        unsigned int const vertLineCount = width() / oneSize;

//        if(vertLineCount > m_dimFull)
//        {
//            line.pos.x -= (vertLineCount - m_dimFull) / 2;
//        }

//        if(horzLineCount > m_dimFull)
//        {
//            line.pos.y -= (horzLineCount - m_dimFull) / 2;
//        }

//        if(line.pos.x < 0 || line.pos.y < 0 || line.pos.x >= (int)m_dimFull || line.pos.y >= (int)m_dimFull)
//        {
//            line.pos.x = 0;
//            line.pos.y = 0;
//            line.orient = LINE::Unknown;
//        }

//        return line;
//    }
//    else
//    {
//        return LINE();
//    }
//}

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // инициализируем устройство рисования и очищем рабочу область перед рисовнием кадра
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // подсвечиваем активный квадрат
    paintCurrentQuadrat(painter);

    // рисуем сетку в центре окна
    paintBackground(painter);

    // сисуем границы игрового поля
    paintBorder(painter);

    // рисуем текущую линию
    paintCurrentLine(painter);
}

void CQuadrats::paintBackground(QPainter& painter)
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

void CQuadrats::paintBorder(QPainter& painter)
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

void CQuadrats::paintCurrentQuadrat(QPainter& painter)
{
    QUADRAT const quadrat = getQuadrat(m_x, m_y);
    fillQuadrat(painter, quadrat, QColor(190, 190, 190, 190));
}

void CQuadrats::paintCurrentLine(QPainter& painter)
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

void CQuadrats::fillQuadrat(QPainter& painter, QUADRAT const& quadrat, QColor const& color)
{
    unsigned int const oneSize = getOneSize();
    painter.fillRect(quadrat.x * oneSize, quadrat.y * oneSize, oneSize, oneSize, color);
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
    QUADRAT const quadrat = getQuadrat(m_x, m_y);
    qDebug() << quadrat.x << quadrat.y << '\n';
//    LINE line;
//    line = getGlobalLine(m_x, m_y);
//    qDebug() << line.pos.x << line.pos.y << line.orient;
//    line = translateLine(line);
//    qDebug() << line.pos.x << line.pos.y << line.orient;
//    qDebug() << '\n';
    #endif
}
