#include <cassert>
#include <quadratsgame.h>
#include <transform.h>
#include <version.h>

QuadratsGame::QuadratsGame(QWidget* parent):
    QMainWindow(parent), m_timer(this)
{
    // Размер игрового пространства должен быть всегда меньше полного размера
    // пространства. Также игровое поле должно быть нечётным поэтому | 1.
    m_dimFull = 14;
    m_dim = m_dimFull * 0.75;
    m_dim |= 1;

    m_logic.resetState(m_dim);
    m_composer.setLogic(&m_logic);

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

unsigned int QuadratsGame::getOneSize()const
{
    assert(m_dimFull > 0);
    int const size = std::min(width(), height());
    return size / m_dimFull;
}

Quadrat QuadratsGame::getQuadratCentral()const
{
    return Transform::getQuadratGlobal(width() / 2, height() / 2, getOneSize());
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
    Line line = Transform::getLineGlobal(event->x(), event->y(), getOneSize());

    // Линию получили в глобальной системе. Храним в относительной поэтому
    // сперва должны преобразовать.
    line = Transform::toLocal(line, getQuadratCentral());

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

    m_composer.setDimensionFull(m_dimFull);
    m_composer.setSize(width(), height());
    m_composer.setPosition(m_x, m_y);

    m_composer.compose();

    QImage const& image = m_composer.getImage();

    QPainter painter(this);
    painter.drawImage(0, 0, image);
}
