// rectangle.cpp
#include "rectangle.h"

Rectangle::Rectangle(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height)
{
}

void Rectangle::draw(QPainter &painter)
{
    painter.setBrush(Qt::blue); // Устанавливаем синий цвет для заливки
    painter.drawRect(x, y, width, height);
}
