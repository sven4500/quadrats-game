#include "CQuadrats.h"

QColor const CQuadrats::sm_backgroundColor = QColor(250, 254, 254);
QColor const CQuadrats::sm_lineColor = QColor(172, 222, 254);
QColor const CQuadrats::sm_activeLineColor = QColor(84, 152, 250);

CQuadrats::CQuadrats(QWidget* parent): QMainWindow(parent)
{
    m_dimension.setWidth(16);
    m_dimension.setHeight(16);
    setMouseTracking(true);
}

CQuadrats::~CQuadrats()
{
}

void CQuadrats::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), sm_backgroundColor);
    // рисуем сетку
    {
        unsigned int const horzLineCount = height() / m_dimension.height(), vertLineCount = width() / m_dimension.width();
        QVector<QPoint> grid;
        grid.reserve(horzLineCount * 2 + vertLineCount * 2);
        for(unsigned int i = 1; i <= horzLineCount; ++i)
        {
            unsigned int const pos = m_dimension.height() * i;
            grid.append(QPoint(0, pos));
            grid.append(QPoint(width(), pos));
        }
        for(unsigned int i = 1; i <= vertLineCount; ++i)
        {
            unsigned int const pos = m_dimension.width() * i;
            grid.append(QPoint(pos, 0));
            grid.append(QPoint(pos, height()));
        }
        painter.setPen(sm_lineColor);
        painter.drawLines(grid);
    }
    // рисуем линию на которую указывает указатель мышт
    {
        unsigned int const horzLine = m_y / m_dimension.height();
        unsigned int const vertLine = m_x / m_dimension.width();
        unsigned int const horzLineMod = m_y % m_dimension.height();
        unsigned int const vertLineMod = m_x % m_dimension.width();
        QPen pen;
        pen.setColor(sm_activeLineColor);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(vertLine * m_dimension.width(), horzLine * m_dimension.height(),
                         vertLine * (m_dimension.width() + 1), horzLine * (m_dimension.height() + 1));
    }
}

void CQuadrats::mouseMoveEvent(QMouseEvent* event)
{
    //Q_UNUSED(event);
    m_x = event->x();
    m_y = event->y();
    update();
}
