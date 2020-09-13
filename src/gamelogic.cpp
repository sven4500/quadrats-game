#include <gamelogic.h>
#include <transform.h>

GameLogic::GameLogic()
{}

GameLogic::~GameLogic()
{}

void GameLogic::resetState(int dim)
{
    _currentPlayer = Player::P1;

    _dim = dim;
    auto half = dim / 2;

    // В начале игры оба игрока имеют по два захваченных квадрата на вершинах.
    Quadrat const leftQuad{-half, 0, Quadrat::Local},
        rightQuad{half, 0, Quadrat::Local},
        upperQuad{0, -half, Quadrat::Local},
        lowerQuad{0, half, Quadrat::Local};

    Line const leftLine = Transform::toLine(leftQuad, Line::Left),
        rightLine = Transform::toLine(rightQuad, Line::Right),
        upperLine = Transform::toLine(upperQuad, Line::Up),
        lowerLine = Transform::toLine(lowerQuad, Line::Down);

    _stats[P1].playerColor = QColor(0, 162, 232);
    _stats[P1].quadrats.push_back(upperQuad);
    _stats[P1].quadrats.push_back(lowerQuad);
    _stats[P1].lines.push_back(upperLine);
    _stats[P1].lines.push_back(lowerLine);

    _stats[P2].playerColor = QColor(237, 28, 36);
    _stats[P2].quadrats.push_back(leftQuad);
    _stats[P2].quadrats.push_back(rightQuad);
    _stats[P2].lines.push_back(leftLine);
    _stats[P2].lines.push_back(rightLine);
}

void GameLogic::addLine(Line const& line)
{
    assert(line.origin == Line::Local);

    if(isInside(line) == true)
    {
        if(isPlayerAcquired(line) == false)
        {
            _stats[_currentPlayer].lines.append(line);

            // Попадаем внутрь функции которая проверяет закрывает добавленная
            // линия квадрат или нет.
            if(tryToEnclose(line) == false)
                _currentPlayer = otherPlayer();
        }
    }
}

int GameLogic::dimension()const
{
    return _dim;
}

QVector<Quadrat> const& GameLogic::quadrats(Player player)const
{
    return _stats[player].quadrats;
}

QVector<Line> const& GameLogic::lines(Player player)const
{
    return _stats[player].lines;
}

bool GameLogic::isInside(Quadrat const& quad)const
{
    assert(quad.origin == Quadrat::Local);

    auto const half = _dim / 2;
    return std::abs(quad.x) + std::abs(quad.y) <= half;
}

bool GameLogic::isInside(Line const& line)const
{
    assert(line.origin == Line::Local);

    auto half = _dim / 2;

    if(line.x <= 0 && line.y >= 0)
    {
        return std::abs(line.x) + std::abs(line.y) < half;
    }
    else if(line.x > 0 && line.y < 0)
    {
        return std::abs(line.x) + std::abs(line.y) <= half;
    }
    else if(line.x > 0 && line.y >= 0)
    {
        switch(line.orientation)
        {
        case Line::Orientation::Horizontal:
            return std::abs(line.x) + std::abs(line.y) < half;
        case Line::Orientation::Vertical:
            return std::abs(line.x) + std::abs(line.y) <= half;
        default:
            return false;
        }
    }
    else if(line.x <= 0 && line.y < 0)
    {
        switch(line.orientation)
        {
        case Line::Orientation::Horizontal:
            return std::abs(line.x) + std::abs(line.y) <= half;
        case Line::Orientation::Vertical:
            return std::abs(line.x) + std::abs(line.y) < half;
        default:
            return false;
        }
    }

    return false;
}

bool GameLogic::isPlayerAcquired(Line const& line)const
{
    return _stats[PlayerOne].contains(line) == true ||
        _stats[PlayerTwo].contains(line) == true;
}

bool GameLogic::isGameBorder(Line const& line)const
{
    assert(line.origin == Line::Local);
    assert(_dim % 2 == 1);

    if(line.orientation == Line::Horizontal)
        return std::abs(line.x) + std::abs(line.y) ==
            _dim / 2 + (line.y < 0 ? 1 : 0);
    else
    if(line.orientation == Line::Vertical)
        return std::abs(line.x) + std::abs(line.y) ==
            _dim / 2 + (line.x > 0 ? 1 : 0);
    else
        return false;
}

GameLogic::Player GameLogic::currentPlayer()const
{
    return _currentPlayer;
}

GameLogic::Player GameLogic::otherPlayer()const
{
    return (_currentPlayer == Player::P1) ? Player::P2 : Player::P1;
}

QColor GameLogic::playerColor(Player player)const
{
    return _stats[player].playerColor;
}

bool GameLogic::tryToEnclose(Quadrat const& quad)const
{
    Line const lines[4] = {
        Transform::toLine(quad, Line::Up),
        Transform::toLine(quad, Line::Down),
        Transform::toLine(quad, Line::Left),
        Transform::toLine(quad, Line::Right)
    };

    bool isEnclosed = true;

    for(auto const& line: lines)
        isEnclosed = isEnclosed && (_stats[P1].contains(line) || _stats[P2].contains(line) || isGameBorder(line));

    return isEnclosed;
}

bool GameLogic::tryToEnclose(Line const& line)
{
    Quadrat quad[2];

    if(line.orientation == Line::Horizontal)
    {
        quad[0] = Transform::toQuadrat(line, Line::Up);
        quad[1] = Transform::toQuadrat(line, Line::Down);
    }
    else
    {
        quad[0] = Transform::toQuadrat(line, Line::Left);
        quad[1] = Transform::toQuadrat(line, Line::Right);
    }

    bool isEnclosed = false;

    if(tryToEnclose(quad[0]) == true)
    {
        _stats[_currentPlayer].quadrats.append(quad[0]);
        isEnclosed = true;
    }

    if(tryToEnclose(quad[1]) == true)
    {
        _stats[_currentPlayer].quadrats.append(quad[1]);
        isEnclosed = true;
    }

    return isEnclosed;
}
