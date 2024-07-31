// secondwindow.cpp
#include "secondwindow.h"

SecondWindow::SecondWindow(QWidget *parent)
    : QWidget(parent),
      clickCount(0),
      isColored(false),
      rectangle(nullptr),
      pixmap(size())
{
    setMinimumSize(400, 300);
}

SecondWindow::~SecondWindow()
{
    delete rectangle;
}

void SecondWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        clickCount++;
        if (clickCount == 1) {
            isColored = true;
        } else if (clickCount == 2) {
            if (!rectangle) {
                rectangle = new Rectangle(50, 50, 100, 50);
            }
        } else if (clickCount == 3) {
            isColored = false;
            delete rectangle;
            rectangle = nullptr;
            clickCount = 0;
        }
        updatePixmap();
        update();
    }
}

void SecondWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}

void SecondWindow::updatePixmap()
{
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    if (isColored) {
        painter.fillRect(rect(), Qt::yellow);
    }
    if (rectangle) {
        rectangle->draw(painter);
    }
}
