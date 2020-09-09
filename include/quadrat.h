#pragma once
#ifndef QUADRAT_H_
#define QUADRAT_H_

// Структура хранит информацию о местоположении одного квадрата.
struct Quadrat
{
    // Система отсчёта. Относительно левого верхнего угла (Global), либо
    // относительно центра игрового поля (Local).
    enum Origin{Local = 0, Global};

    Quadrat():
        origin(Global), x(0), y(0)
    {

    }

    Quadrat(int x, int y, Origin origin = Global):
        origin(origin), x(x), y(y)
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

    inline bool operator==(Quadrat const& other)const
    {
        assert(origin == other.origin);
        return x == other.x && y == other.y;
    }

    Origin origin;
    int x;
    int y;

};

#endif
