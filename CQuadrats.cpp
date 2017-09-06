#include "CQuadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);
QColor const CQuadrats::sm_sideLineColor = QColor(243, 22, 72);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    m_dim = 20; // по-умолчанию сетка 20x20

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

    setWindowTitle("Квадраты (версия 0.13)");
}

CQuadrats::~CQuadrats()
{
}

LINE CQuadrats::getLine(int x, int y)const
{
    unsigned int const oneSize = std::min(width(), height()) / m_dim;

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

    line.index.x = x / oneSize;
    line.index.y = y / oneSize;

    switch(orientation)
    {
    case 0:
        line.orientation = LINE::Left;
        break;
    case 1:
        line.orientation = LINE::Up;
        break;
    case 2:
        line.orientation = LINE::Right;
        break;
    case 3:
        line.orientation = LINE::Down;
        break;
    default:
        line.orientation = LINE::Unknown;
        break;
    }

    return line;
}

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);

    // размер одного квадрата в пикселях
    unsigned int const oneSize = std::min(width(), height()) / m_dim;

    // рисуем сетку в центре окна
    {
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
        painter.setPen(sm_sideLineColor);
        painter.drawLine(oneSize * 4.5, 0, oneSize * 4.5, height());
    }

    // рисуем линию на которую указывает указатель мышт
    {
        LINE line;
        line = getLine(m_x, m_y);

        QPen pen;
        pen.setColor(sm_activeLineColor);
        pen.setWidth(2);
        painter.setPen(pen);

        switch(line.orientation)
        {
        case LINE::Left:
            painter.drawLine(line.index.x * oneSize, line.index.y * oneSize,
                line.index.x * oneSize, (line.index.y + 1) * oneSize);
            break;
        case LINE::Up:
            painter.drawLine(line.index.x * oneSize, line.index.y * oneSize,
                (line.index.x + 1) * oneSize, line.index.y * oneSize);
            break;
        case LINE::Right:
            painter.drawLine((line.index.x + 1) * oneSize, line.index.y * oneSize,
                (line.index.x + 1) * oneSize, (line.index.y + 1) * oneSize);
            break;
        case LINE::Down:
            painter.drawLine(line.index.x * oneSize, (line.index.y + 1) * oneSize,
                (line.index.x + 1) * oneSize, (line.index.y + 1) * oneSize);
            break;
        default:
            break;
        };
    }
}

void CQuadrats::mouseMoveEvent(QMouseEvent* event)
{
    //Q_UNUSED(event);

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
}
