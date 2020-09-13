#include <cassert>
#include <QPainter>
#include <QRect>
#include <QPen>
#include <QBrush>
#include <imagecomposer.h>
#include <transform.h>

QColor const ImageComposer::_backgroundColor = QColor(250, 254, 254);
QColor const ImageComposer::_gridColor = QColor(172, 222, 254);

ImageComposer::ImageComposer():
    _logic(nullptr), _dimFull(0)
{
    // Формируем список методов для отрисовки в порядке добавления.
    _painterFuncs.append(&paintBackgroud);
    _painterFuncs.append(&paintCurrentQuadrat);
    _painterFuncs.append(&paintGrid);
    _painterFuncs.append(&paintCapturedQuadrats);
    _painterFuncs.append(&paintPlayerLines);
    _painterFuncs.append(&paintCurrentLine);
    _painterFuncs.append(&paintGameBorder);
    _painterFuncs.append(&paintScore);
}

ImageComposer::~ImageComposer()
{}

void ImageComposer::compose()
{
    _image = QImage(_width, _height, QImage::Format_RGB32);

    if(_width <= 0 || _height <= 0)
        return;

    QPainter painter(&_image);

    for(int i = 0; i < _painterFuncs.size(); ++i)
    {
        PainterFunc const& painterFunc = _painterFuncs[i];

        painter.save();
        (this->*painterFunc)(painter);
        painter.restore();
    }
}

QImage const& ImageComposer::getImage()const
{
    return _image;
}

void ImageComposer::setLogic(GameLogic const* logic)
{
    _logic = logic;
}

GameLogic const* ImageComposer::getLogic()const
{
    return _logic;
}

void ImageComposer::setDimensionFull(int dimFull)
{
    _dimFull = dimFull;
}

void ImageComposer::setSize(int width, int height)
{
    _width = width;
    _height = height;
}

void ImageComposer::setPosition(int x, int y)
{
    _x = x;
    _y = y;
}

Quadrat ImageComposer::getQuadratCentral()const
{
    auto const oneSize = getOneSize();
    return Transform::getQuadratGlobal(_width / 2, _height / 2, oneSize);
}

int ImageComposer::getOneSize()const
{
    assert(_dimFull > 0);
    auto const size = std::min(_width, _height);
    return size / _dimFull;
}

void ImageComposer::paintBackgroud(QPainter& painter)const
{
    painter.fillRect(0, 0, _width, _height, _backgroundColor);
}

void ImageComposer::paintGrid(QPainter& painter)const
{
    // размер одного квадрата в пикселях
    auto const oneSize = getOneSize();

    // сюда будем сохранять пары точек начала и конца каждой линии сетки
    QVector<QPoint> grid;

    // количество горизонтальных и вертикальных линий
    auto const horzLineCount = _height / oneSize;
    auto const vertLineCount = _width / oneSize;

    grid.reserve(horzLineCount * 2 + vertLineCount * 2);

    for(auto i = 1; i <= horzLineCount; ++i)
    {
        auto const pos = oneSize * i;
        grid.append(QPoint(0, pos));
        grid.append(QPoint(_width, pos));
    }

    for(auto i = 1; i <= vertLineCount; ++i)
    {
        auto const pos = oneSize * i;
        grid.append(QPoint(pos, 0));
        grid.append(QPoint(pos, _height));
    }

    painter.setPen(_gridColor);
    painter.drawLines(grid);
}

void ImageComposer::paintGameBorder(QPainter& painter)const
{
    auto const oneSize = getOneSize();

    // Количество горизонтальных и вертикальных линий.
    auto const horzLineCount = _height / oneSize;
    auto const vertLineCount = _width / oneSize;

    // Находим центр игрового поля.
    auto const ix = vertLineCount / 2;
    auto const iy = horzLineCount / 2;

    auto const half = _logic->dimension() / 2;

    QPen pen;
    pen.setColor(QColor(64, 64, 64));
    pen.setWidth(2);

    painter.setPen(pen);

    for(auto i = 0; i <= half; ++i)
    {
        int const x[4] = {
            (ix - i) * oneSize,
            (ix + i + 1) * oneSize,
            x[0] + oneSize,
            x[1] - oneSize
        };

        int const y[4] = {
            (iy - half + i) * oneSize,
            (iy + half - i + 1) * oneSize,
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

void ImageComposer::paintCurrentQuadrat(QPainter& painter)const
{
    Quadrat const quadGlobal = Transform::getQuadratGlobal(_x, _y, getOneSize());
    Quadrat const quad = Transform::toLocal(quadGlobal, getQuadratCentral());

    if(_logic->isInside(quad) == true)
    {
        drawQuadrat(painter, quadGlobal, QColor(190, 190, 190, 190));
    }
}

void ImageComposer::paintCurrentLine(QPainter& painter)const
{
    Line const lineGlobal = Transform::getLineGlobal(_x, _y, getOneSize());
    Line const line = Transform::toLocal(lineGlobal, getQuadratCentral());

    PlayerEnum const currentPlayer = _logic->currentPlayer(),
        otherPlayer = _logic->otherPlayer();

    if(_logic->lines(otherPlayer).contains(line) == false &&
        _logic->isInside(line) == true)
    {
        drawLine(painter, line, _logic->playerColor(currentPlayer));
    }
}

void ImageComposer::paintPlayerLines(QPainter& painter)const
{
    auto const& linesP1 = _logic->lines(PlayerEnum::PlayerOne);
    auto const& colorP1 = _logic->playerColor(PlayerEnum::PlayerOne);

    for(int j = 0; j < linesP1.size(); ++j)
    {
        drawLine(painter, linesP1[j], colorP1);
    }

    auto const& linesP2 = _logic->lines(PlayerEnum::PlayerTwo);
    auto const& colorP2 = _logic->playerColor(PlayerEnum::PlayerTwo);

    for(int j = 0; j < linesP2.size(); ++j)
    {
        drawLine(painter, linesP2[j], colorP2);
    }
}

void ImageComposer::paintCapturedQuadrats(QPainter& painter)const
{
    auto const& quadP1 = _logic->quadrats(PlayerEnum::PlayerOne);

    for(int i = 0; i < quadP1.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadP1[i], PlayerEnum::PlayerOne);
    }

    auto const& quadP2 = _logic->quadrats(PlayerEnum::PlayerTwo);

    for(int i = 0; i < quadP2.size(); ++i)
    {
        drawCapturedQuadrat(painter, quadP2[i], PlayerEnum::PlayerTwo);
    }
}

void ImageComposer::paintScore(QPainter& painter)const
{
    painter.setFont(QFont("Courier New", 16, QFont::Normal));

    drawPlayerScore(painter, PlayerEnum::P1);
    drawPlayerScore(painter, PlayerEnum::P2);
}

void ImageComposer::drawQuadrat(QPainter& painter, Quadrat const& quad, QColor const& color, Qt::BrushStyle style)const
{
    Quadrat const quadGlobal = Transform::toGlobal(quad, getQuadratCentral());
    auto const oneSize = getOneSize();

    QBrush brush;
    brush.setColor(color);
    brush.setStyle(style);

    painter.fillRect(quadGlobal.x * oneSize, quadGlobal.y * oneSize, oneSize, oneSize, brush);
}

void ImageComposer::drawLine(QPainter& painter, Line const& line, QColor const& color)const
{
    Line const lineGlobal = Transform::toGlobal(line, getQuadratCentral());

    auto const oneSize = getOneSize();
    auto const ix = lineGlobal.x * oneSize;
    auto const iy = lineGlobal.y * oneSize;

    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    switch(lineGlobal.orientation)
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

void ImageComposer::drawPlayerScore(QPainter& painter, PlayerEnum player)const
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
    if(player == PlayerEnum::PlayerTwo)
    {
        drawRect = &br2;

        drawRect->setLeft(_width - drawRect->width() - corner.x());
        drawRect->setRight(_width - corner.x());

        flags |= Qt::AlignRight;
    }

    assert(drawRect != nullptr);

    QString const scoreText = QString("%1 | %2")
        .arg(_logic->quadrats(player).size())
        .arg(_logic->lines(player).size());

    QRect const marginRect = drawRect->adjusted(-5, -5, 5, 5);

    painter.setPen(QPen(_logic->playerColor(player), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));

    painter.drawRect(marginRect);
    painter.drawText(*drawRect, flags, scoreText, drawRect);
}

void ImageComposer::drawCapturedQuadrat(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    QColor const color = _logic->playerColor(player);
    drawQuadrat(painter, quad, color, Qt::Dense6Pattern);
    drawPlayerInsignia(painter, quad, player);
}

void ImageComposer::drawPlayerInsignia(QPainter& painter, Quadrat const& quad, PlayerEnum player)const
{
    if(player == PlayerEnum::PlayerOne)
        drawCross(painter, quad, _logic->playerColor(player));
    else
    if(player == PlayerEnum::PlayerTwo)
        drawCircle(painter, quad, _logic->playerColor(player));
    else
        assert(false);
}

void ImageComposer::drawCross(QPainter& painter, Quadrat const& quad, QColor const& color)const
{
    auto const oneSize = getOneSize();

    auto const margin = oneSize * 0.25;
    auto const size = oneSize - margin * 2;

    Quadrat const global = Transform::toGlobal(quad, getQuadratCentral());

    QRect const rect(global.x * oneSize + margin + 1, global.y * oneSize + margin + 1, size, size);

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

void ImageComposer::drawCircle(QPainter& painter, Quadrat const& quad, QColor const& color)const
{
    auto const oneSize = getOneSize();

    auto const margin = oneSize * 0.25;
    auto const size = oneSize - margin * 2;

    Quadrat const global = Transform::toGlobal(quad, getQuadratCentral());

    QRect const rect(global.x * oneSize + margin + 1, global.y * oneSize + margin + 1, size - 1, size - 1);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(color);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    painter.drawEllipse(rect);
}
