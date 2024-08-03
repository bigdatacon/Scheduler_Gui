#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QPainter>

class Rectangle
{
public:
    Rectangle(int x, int y, int width, int height);
    void draw(QPainter &painter);
    void setSize(int width, int height);
    void setPosition(int x, int y);

    int x;
    int y;
    int width;
    int height;
};

#endif // RECTANGLE_H
