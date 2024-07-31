// secondwindow.cpp
#include "secondwindow.h"

SecondWindow::SecondWindow(QWidget *parent)
    : QWidget(parent),
      clickCount(0),
      isColored(false),
      rectangle(nullptr),
      pixmap(size())
{
    setAttribute(Qt::WA_TranslucentBackground, true); // Устанавливаем прозрачный фон
    updateInnerRect();
}

SecondWindow::~SecondWindow()
{
    delete rectangle;
}

void SecondWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && innerRect.contains(event->pos())) {
        clickCount++;
        if (clickCount == 1) {
            isColored = true;
        } else if (clickCount == 2) {
            if (!rectangle) {
                rectangle = new Rectangle(innerRect.width() / 4, innerRect.height() / 4, innerRect.width() / 2, innerRect.height() / 2);
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

void SecondWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    pixmap = QPixmap(size());
    updateInnerRect();
    updatePixmap();
}

void SecondWindow::updateInnerRect()
{
    int margin = 20;
    int innerWidth = width() / 2 - margin;
    int innerHeight = height() / 2 - margin;
    innerRect = QRect((width() - innerWidth) / 2, (height() - innerHeight) / 2, innerWidth, innerHeight);
}

void SecondWindow::updatePixmap()
{
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawRect(innerRect.adjusted(0, 0, -1, -1)); // Рисуем границу внутреннего окна
    if (isColored) {
        painter.fillRect(innerRect.adjusted(1, 1, -1, -1), Qt::yellow);
    }
    if (rectangle) {
        painter.save();
        painter.translate(innerRect.topLeft());
        rectangle->draw(painter);
        painter.restore();
    }
}
