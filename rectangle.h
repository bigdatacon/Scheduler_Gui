// rectangle.h
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QPainter>

class Rectangle
{
public:
    Rectangle(int x, int y, int width, int height);
    void draw(QPainter &painter);

private:
    int x, y, width, height;
};

#endif // RECTANGLE_H
