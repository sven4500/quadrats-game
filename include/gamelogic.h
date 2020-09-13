#pragma once
#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_

#include <QVector>
#include <QColor>
#include <quadrat.h>
#include <line.h>
#include <playerstats.h>

class GameLogic
{
public:
    enum Player{PlayerOne = 0, PlayerTwo = 1, P1 = 0, P2 = 1};

    GameLogic();
    ~GameLogic();

    void resetState(int dim);
    void addLine(Line const& line);

    int dimension()const;

    QVector<Quadrat> const& quadrats(Player player)const;
    QVector<Line> const& lines(Player player)const;

    bool isInside(Quadrat const& quadrat)const;
    bool isInside(Line const& line)const;

    bool isPlayerAcquired(Line const& line)const;
    bool isGameBorder(Line const& line)const;

    Player currentPlayer()const;
    Player otherPlayer()const;

    QColor playerColor(Player player)const;

protected:
    bool tryToEnclose(Quadrat const& quad)const;
    bool tryToEnclose(Line const& line);

    PlayerStats _stats[2];
    Player _currentPlayer;

    int _dim;

};

#endif
