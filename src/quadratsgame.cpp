#include <cassert>
#include <QIcon>
#include <QMessageBox>
#include <quadratsgame.h>
#include <transform.h>
#include <version.h>

QuadratsGame::QuadratsGame(QWidget* parent):
    QMainWindow(parent), m_settingsDialog(this), m_timer(this),
    m_isNetModeOn(false)
{
    // Размер игрового пространства должен быть всегда меньше полного размера
    // пространства. Также игровое поле должно быть нечётным поэтому | 1.
    m_dimFull = 14;
    m_dim = m_dimFull * 0.75;
    m_dim |= 1;

    m_logic.resetState(m_dim);
    m_composer.setLogic(&m_logic);
    connect(&m_netCoop, &NetCoop::dataRead, this, &QuadratsGame::dataReady);

    QMenuBar* const menu = menuBar();

    m_newGameAct = menu->addAction("");
    m_newGameAct->setToolTip("Новая игра");
    m_newGameAct->setIcon(QIcon(":/new-game.ico"));

    m_joinGameAct = menu->addAction("");
    m_joinGameAct->setToolTip("Присоединиться");
    m_joinGameAct->setIcon(QIcon(":/join.ico"));
    connect(m_joinGameAct, &QAction::triggered, this, &QuadratsGame::joinGame);

    m_createGameAct = menu->addAction("");
    m_createGameAct->setToolTip("Создать игру");
    m_createGameAct->setIcon(QIcon(":/host.ico"));
    connect(m_createGameAct, &QAction::triggered, this, &QuadratsGame::hostGame);

    m_settingsAct = menu->addAction("");
    m_settingsAct->setToolTip("Настройки");
    m_settingsAct->setIcon(QIcon(":/settings.ico"));
    connect(m_settingsAct, &QAction::triggered, &m_settingsDialog, &QDialog::exec);

    m_howToAct = menu->addAction("Как играть");
    m_howToAct->setToolTip("Как играть");
    m_howToAct->setIcon(QIcon(":/how-to-play.ico"));

    m_aboutAct = menu->addAction("");
    m_aboutAct->setToolTip("О программе");
    m_aboutAct->setIcon(QIcon(":/about.ico"));

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

void QuadratsGame::hostGame()
{
    m_isNetModeOn = true;
    m_player = PlayerEnum::P1;

    m_logic.resetState(m_settingsDialog.getDim());

    auto value = m_netCoop.hostGame(m_settingsDialog.getPort());

    if(value == false)
        QMessageBox::critical(this, "Ошибка", "Не удалось создать игру.");
    else
        QMessageBox::information(this, "Успех", "Игра создана!");
}

void QuadratsGame::joinGame()
{
    m_isNetModeOn = true;
    m_player = PlayerEnum::P2;

    m_logic.resetState(m_settingsDialog.getDim());

    auto value = m_netCoop.joinGame(m_settingsDialog.getIpAddress(), m_settingsDialog.getPort());

    if(value == false)
        QMessageBox::critical(this, "Ошибка", "Не удалось установить соединение.");
    else
        QMessageBox::information(this, "Успех", "Соединение установлено!");
}

void QuadratsGame::dataReady(Line const& line)
{
    m_logic.addLine(line);
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
    if(m_isNetModeOn == false || m_logic.currentPlayer() == m_player)
    {
        Line line = Transform::getLineGlobal(event->x(), event->y(), getOneSize());

        // Линию получили в глобальной системе. Храним в относительной поэтому
        // сперва должны преобразовать.
        line = Transform::toLocal(line, getQuadratCentral());

        m_logic.addLine(line);

        if(m_isNetModeOn == true)
        {
            m_netCoop.dataWrite(line);
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

    m_composer.setDimensionFull(m_dimFull);
    m_composer.setSize(width(), height());
    m_composer.setPosition(m_x, m_y);

    m_composer.compose();

    QImage const& image = m_composer.getImage();

    QPainter painter(this);
    painter.drawImage(0, 0, image);
}
