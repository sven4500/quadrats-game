#pragma once
#ifndef PLAYERSTATS_H_
#define PLAYERSTATS_H_

#include <QVector>
#include <QColor>
#include <quadrat.h>
#include <line.h>

struct PlayerStats
{
    inline bool contains(Line const& line)const
    {
        return lines.contains(line);
    }

    inline bool contains(Quadrat const& quadrat)const
    {
        return quadrats.contains(quadrat);
    }

    void clear()
    {
        lines.clear();
        quadrats.clear();
    }

    QVector<Line> lines;
    QVector<Quadrat> quadrats;
    QColor playerColor;

};

#endif
