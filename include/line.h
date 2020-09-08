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

    Origin origin;
    Orientation orientation;
    int x;
    int y;

};

#endif
