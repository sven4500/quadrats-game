#pragma once
#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <quadrat.h>
#include <line.h>

class Transform
{
public:
    static Line toLine(Quadrat const& quad, Line::Orientation orientation)
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

    static Line toLine(Quadrat const& quad, Line::Position pos)
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

    // Метод производит преобразование квадрата в линию. Аргумент pos указывает
    // по какую сторону от линии расположить квадрат (в зависимости от
    // расположеия линии горизонтального или вертикального).
    static Quadrat toQuadrat(Line const& line, Line::Position pos = Line::Down)
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

private:
    Transform()
    {}

    ~Transform()
    {}

};

#endif
