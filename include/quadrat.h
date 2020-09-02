#pragma once
#ifndef QUADRAT_H_
#define QUADRAT_H_

// Структура хранит информацию о местоположении одного квадрата.
struct QUADRAT
{
    // Система отсчёта. Относительно левого верхнего угла (Global), либо относительно центра игрового поля (Local).
    enum Origin{Local = 0, Global};

    QUADRAT():
        origin(Global), x(0), y(0)
    {

    }

    QUADRAT(int x, int y, Origin origin = Global):
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

    inline bool operator==(QUADRAT const& other)const
    {
        return x == other.x && y == other.y;
    }

    Origin origin;  // Система отсчёта: локальная или глобальная.
    int x;          // Координата квадрата.
    int y;          // Координата квадрата.

};

#endif
