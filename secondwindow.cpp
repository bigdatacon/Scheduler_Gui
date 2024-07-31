#include "secondwindow.h"

SecondWindow::SecondWindow(QWidget *parent, int width, int height)
    : QWidget(parent),
      clickCount(0),
      isColored(false),
      rectangle(nullptr),
      pixmap(size())
{
    setAttribute(Qt::WA_TranslucentBackground, true); // Устанавливаем прозрачный фон
    setStyleSheet("background-color: transparent;"); // Устанавливаем стиль прозрачного фона
    setFixedSize(width, height); // Устанавливаем фиксированный размер окна
    updateInnerRect();
    drawInnerPixmap();
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
        drawInnerPixmap();
        update();
    }
}

void SecondWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // Создаем объект QPainter для рисования на виджете
    painter.drawPixmap(0, 0, pixmap); // Отображаем pixmap на виджете
    painter.drawPixmap(innerRect.topLeft(), innerPixmap); // Отображаем innerPixmap на виджете
}

void SecondWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    pixmap = QPixmap(size());
    pixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для pixmap
    updateInnerRect();
    drawInnerPixmap();
}

void SecondWindow::updateInnerRect()
{
    int margin = 20;
    int innerWidth = width() / 2 - margin;
    int innerHeight = height() / 2 - margin;
    innerRect = QRect((width() - innerWidth) / 2, (height() - innerHeight) / 2, innerWidth, innerHeight);
    innerPixmap = QPixmap(innerRect.size()); // Обновляем размер innerPixmap
    innerPixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для innerPixmap
}

void SecondWindow::drawInnerPixmap()
{
    innerPixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для innerPixmap
    QPainter painter(&innerPixmap);
    painter.setPen(Qt::black);
    painter.drawRect(innerPixmap.rect().adjusted(0, 0, -1, -1)); // Рисуем границу внутреннего окна
    if (isColored) {
        painter.fillRect(innerPixmap.rect().adjusted(1, 1, -1, -1), Qt::yellow); // Заливаем внутреннее окно желтым цветом
    }
    if (rectangle) {
        rectangle->draw(painter); // Рисуем прямоугольник внутри innerPixmap
    }
}
