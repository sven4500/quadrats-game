#pragma once
#ifndef PLAYER_H_
#define PLAYER_H_

#include <QVector>
#include <QColor>
#include <quadrat.h>
#include <line.h>

// структура хранит ходы одного игрока
struct Player
{
    inline bool contains(Line const& line)const
    {
        return lines.contains(line);
    }

    inline bool contains(Quadrat const& quadrat)const
    {
        return quadrats.contains(quadrat);
    }

    QVector<Line> lines;
    QVector<Quadrat> quadrats;
    QColor playerColor;

};

#endif
