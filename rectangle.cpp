#include "rectangle.h"

Rectangle::Rectangle(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height)
{
}

void Rectangle::draw(QPainter &painter)
{
//    painter.setBrush(QColor(0, 0, 255, 128)); // Синийe цвет с прозрачностью
    painter.setBrush(Qt::blue); // Устанавливаем синий цвет для заливки
    painter.drawRect(x, y, width, height);
}

void Rectangle::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Rectangle::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}
