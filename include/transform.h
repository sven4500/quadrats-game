#pragma once
#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <quadrat.h>
#include <line.h>

class Transform
{
public:
    static Line getLineGlobal(int x, int y, int oneSize);

    static Quadrat getQuadratGlobal(int x, int y, int oneSize);

    static Line toLocal(Line const& line, Quadrat const& central);
    static Line toGlobal(Line const& line, Quadrat const& central);

    static Quadrat toLocal(Quadrat const& quad, Quadrat const& central);
    static Quadrat toGlobal(Quadrat const& quad, Quadrat const& central);

    static Line toLine(Quadrat const& quad, Line::Orientation orientation);
    static Line toLine(Quadrat const& quad, Line::Position pos);

    // Метод производит преобразование квадрата в линию. Аргумент pos указывает
    // по какую сторону от линии расположить квадрат (в зависимости от
    // расположеия линии горизонтального или вертикального).
    static Quadrat toQuadrat(Line const& line, Line::Position pos = Line::Down);

private:
    Transform();
    ~Transform();

};

#endif
