#pragma once
#ifndef LINE_H_
#define LINE_H_

#include <cassert>
#include <quadrat.h>

// Структура хранит информацию о местоположении одной линии.
struct Line
{
    enum Origin{Local = 0, Global};
    enum Orientation{Horizontal = 0, Vertical};
    enum Position{Up = 0, Down, Left, Right};

    Line():
        origin(Global), orientation(Vertical), x(0), y(0)
    {

    }

    Line(int x, int y, Orientation orientation, Origin origin = Global):
        origin(origin), orientation(orientation), x(x), y(y)
    {

    }

    Line(Quadrat const& quad, Orientation orientation):
        orientation(orientation), x(quad.x), y(quad.y)
    {
        if(quad.origin == Quadrat::Global)
            origin = Line::Global;
        else
        if(quad.origin == Quadrat::Local)
            origin = Line::Local;
    }

    Line(Quadrat const& quad, Position pos):
        x(quad.x), y(quad.y)
    {
        if(quad.origin == Quadrat::Global)
            origin = Line::Global;
        else
        if(quad.origin == Quadrat::Local)
            origin = Line::Local;

        if(pos == Line::Up || pos == Line::Down)
            orientation = Line::Horizontal;
        else
        if(pos == Line::Left || pos == Line::Right)
            orientation = Line::Vertical;

        if(pos == Line::Down)
            y -= 1;
        else
        if(pos == Line::Left)
            x += 1;
    }

    inline bool isLocal()const
    {
        return origin == Local;
    }

    inline bool isGlobal()const
    {
        return origin == Global;
    }

    inline bool operator==(Line const& other)const
    {
        assert(origin == other.origin);
        return orientation == other.orientation && x == other.x && y == other.y;
    }

    Origin origin;              // Система координат: локальная или глобальная.
    Orientation orientation;    // Расположение линии: горизонтальное или вертикальное.
    int x;                      // Координата линии.
    int y;                      // Координата линии.

};

#endif
