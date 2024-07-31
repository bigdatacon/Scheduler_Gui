// rectangle.cpp
#include "rectangle.h"

Rectangle::Rectangle(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height)
{
}

void Rectangle::draw(QPainter &painter)
{
    painter.drawRect(x, y, width, height);
}
