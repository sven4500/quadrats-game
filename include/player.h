#pragma once
#ifndef PLAYER_H_
#define PLAYER_H_

#include <QVector>
#include <QColor>
#include <quadrat.h>
#include <line.h>

// структура хранит ходы одного игрока
struct PLAYER_STATS
{
    inline bool contains(LINE const& line)const
    {
        return lines.contains(line);
    }

    inline bool contains(QUADRAT const& quadrat)const
    {
        return quadrats.contains(quadrat);
    }

    QVector<LINE> lines;
    QVector<QUADRAT> quadrats;
    QColor playerColor;

};

#endif
