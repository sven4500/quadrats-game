#pragma once
#ifndef IMAGECOMPOSER_H_
#define IMAGECOMPOSER_H_

#include <QImage>
#include <QVector>
#include <QPainter>
#include <QColor>
#include <quadrat.h>
#include <line.h>
#include <gamelogic.h>

class ImageComposer
{
public:
    ImageComposer();
    ~ImageComposer();

    void compose();
    QImage const& getImage()const;

    void setLogic(GameLogic const* getLogic);
    GameLogic const* getLogic()const;

    void setDimensionFull(int dimFull);

    void setSize(int width, int height);
    void setPosition(int x, int y);

protected:
    typedef GameLogic::Player PlayerEnum;

    // Тип функции которая используется для отрисовки одного слоя изображения.
    typedef void (ImageComposer::* PainterFunc)(QPainter&)const;

    static QColor const _backgroundColor;
    static QColor const _gridColor;

    Quadrat getQuadratCentral()const;

    int getOneSize()const;

    // Список методов отрисовки слоёв изображения.
    void paintBackgroud(QPainter& painter)const;
    void paintGrid(QPainter& painter)const;
    void paintGameBorder(QPainter& painter)const;
    void paintCurrentQuadrat(QPainter& painter)const;
    void paintCurrentLine(QPainter& painter)const;
    void paintPlayerLines(QPainter& painter)const;
    void paintCapturedQuadrats(QPainter& painter)const;
    void paintScore(QPainter& painter)const;

    // заполянет один квадрат заданным цветом
    void drawQuadrat(QPainter& painter, Quadrat const& quadrat, QColor const& color, Qt::BrushStyle style = Qt::SolidPattern)const;
    void drawLine(QPainter& painter, Line const& line, QColor const& color)const;
    void drawCapturedQuadrat(QPainter& painter, Quadrat const&, PlayerEnum player)const;
    void drawPlayerInsignia(QPainter& painter, Quadrat const& quadrat, PlayerEnum player)const;
    void drawCross(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawCircle(QPainter& painter, Quadrat const& quadrat, QColor const& color)const;
    void drawPlayerScore(QPainter& painter, PlayerEnum player)const;

    QImage _image;

    // Вектор слоёв отрисовки изображения.
    QVector<PainterFunc> _painterFuncs;

    // Указатель на класс игровой логики.
    GameLogic const* _logic;

    int _dimFull;

    int _width;
    int _height;

    int _x;
    int _y;

};

#endif
