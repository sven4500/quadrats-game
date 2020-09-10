#include <transform.h>

Transform::Transform()
{}

Transform::~Transform()
{}

Line Transform::toLocal(Line const& line, Quadrat const& cent)
{
    if(line.origin == Line::Local)
        return line;

    Line local = line;

    local.origin = Line::Origin::Local;
    local.orientation = line.orientation;
    local.x = line.x - cent.x;
    local.y = cent.y - line.y;

    return local;
}

Line Transform::toGlobal(Line const& line, Quadrat const& cent)
{
    if(line.origin == Line::Global)
        return line;

    Line global;

    global.origin = Line::Origin::Global;
    global.orientation = line.orientation;
    global.x = line.x + cent.x;
    global.y = cent.y - line.y;

    return global;
}

Quadrat Transform::toLocal(Quadrat const& quad, Quadrat const& cent)
{
    if(quad.origin == Quadrat::Local)
        return quad;

    Quadrat local;

    local.origin = Quadrat::Origin::Local;
    local.x = quad.x - cent.x;
    local.y = cent.y - quad.y;

    return local;
}

Quadrat Transform::toGlobal(Quadrat const& quad, Quadrat const& cent)
{
    if(quad.origin == Quadrat::Global)
        return quad;

    Quadrat global;

    global.origin = Quadrat::Origin::Global;
    global.x = quad.x + cent.x;
    global.y = cent.y - quad.y;

    return global;
}

Line Transform::toLine(Quadrat const& quad, Line::Orientation orientation)
{
    Line line;

    line.x = quad.x;
    line.y = quad.y;
    line.orientation = orientation;

    if(quad.origin == Quadrat::Global)
        line.origin = Line::Global;
    else
    if(quad.origin == Quadrat::Local)
        line.origin = Line::Local;

    return line;
}

Line Transform::toLine(Quadrat const& quad, Line::Position pos)
{
    Line line;

    line.x = quad.x;
    line.y = quad.y;

    if(quad.origin == Quadrat::Global)
        line.origin = Line::Global;
    else
    if(quad.origin == Quadrat::Local)
        line.origin = Line::Local;

    if(pos == Line::Up || pos == Line::Down)
        line.orientation = Line::Horizontal;
    else
    if(pos == Line::Left || pos == Line::Right)
        line.orientation = Line::Vertical;

    if(pos == Line::Down)
        line.y -= 1;
    else
    if(pos == Line::Left)
        line.x += 1;

    return line;
}

Quadrat Transform::toQuadrat(Line const& line, Line::Position pos)
{
    Quadrat quad;

    quad.x = line.x;
    quad.y = line.y;

    if(line.origin == Line::Global)
        quad.origin = Quadrat::Global;
    else
    if(line.origin == Line::Local)
        quad.origin = Quadrat::Local;

    if(line.orientation == Line::Horizontal && pos == Line::Up)
        quad.y += 1;
    else
    if(line.orientation == Line::Vertical && pos == Line::Left)
        quad.x -= 1;

    return quad;
}
