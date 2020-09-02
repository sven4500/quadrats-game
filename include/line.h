#pragma once
#ifndef LINE_H_
#define LINE_H_

// Структура хранит информацию о местоположении одной линии.
struct LINE
{
    enum Origin{Local = 0, Global};
    enum Orientation{Horizontal = 0, Vertical};

    LINE():
        origin(Global), orientation(Vertical), x(0), y(0)
    {

    }

    LINE(int x, int y, Orientation orientation, Origin origin = Global):
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

    inline bool operator==(LINE const& other)const
    {
        return orientation == other.orientation && x == other.x && y == other.y;
    }

    Origin origin;              // Система координат: локальная или глобальная.
    Orientation orientation;    // Расположение линии: горизонтальное или вертикальное.
    int x;                      // Координата линии.
    int y;                      // Координата линии.

};

#endif
