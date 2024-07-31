#include "secondwindow.h"

SecondWindow::SecondWindow(QWidget *parent, int width, int height)
    : QWidget(parent),
      clickCount(0),
      isColored(false),
      rectangle(nullptr),
      rectangleVisible(false),
      pixmap(size())
{
    setAttribute(Qt::WA_TranslucentBackground, true); // Устанавливаем прозрачный фон
    setStyleSheet("background-color: transparent;"); // Устанавливаем стиль прозрачного фона
    setFixedSize(width, height); // Устанавливаем фиксированный размер окна
    updateInnerRect();
    initializePixmap();
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
            drawBasePixmap();
            // После первого клика достаточно обновить только basePixmap
        } else if (clickCount == 2) {
            if (!rectangle) {
                rectangle = new Rectangle(innerRect.width() / 4, innerRect.height() / 4, innerRect.width() / 2, innerRect.height() / 2);
                drawRectanglePixmap();
            }
            rectangleVisible = true;
            updateDrawing();
        } else if (clickCount == 3) {
            isColored = false;
            rectangleVisible = false;
            delete rectangle;
            rectangle = nullptr;
            clickCount = 0;
            restoreBasePixmap();
        }
        update(); // Обновляем виджет для отрисовки
    }
}

void SecondWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // Создаем объект QPainter для рисования на виджете
    painter.drawPixmap(0, 0, pixmap); // Отображаем pixmap на виджете
    painter.drawPixmap(innerRect.topLeft(), innerPixmap); // Отображаем innerPixmap на виджете
    Q_UNUSED(event);
}

void SecondWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    pixmap = QPixmap(size());
    pixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для pixmap
    updateInnerRect();
    initializePixmap();
    update();
}

void SecondWindow::initializePixmap()
{
    innerPixmap = QPixmap(innerRect.size());
    innerPixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для innerPixmap
    basePixmap = QPixmap(innerRect.size());
    basePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для basePixmap
    drawBasePixmap(); // Рисуем базовый innerPixmap
    innerPixmap = basePixmap.copy(); // Сохраняем базовое изображение в innerPixmap
    rectanglePixmap = QPixmap(innerRect.size());
    rectanglePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для rectanglePixmap
}

void SecondWindow::updateInnerRect()
{
    int margin = 20;
    int innerWidth = width() / 2 - margin;
    int innerHeight = height() / 2 - margin;
    innerRect = QRect((width() - innerWidth) / 2, (height() - innerHeight) / 2, innerWidth, innerHeight);
}

void SecondWindow::drawBasePixmap()
{
    basePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для basePixmap
    QPainter painter(&basePixmap);
    painter.setPen(Qt::black);
    painter.drawRect(basePixmap.rect().adjusted(0, 0, -1, -1)); // Рисуем границу внутреннего окна
    if (isColored) {
        painter.fillRect(basePixmap.rect().adjusted(1, 1, -1, -1), Qt::yellow); // Заливаем внутреннее окно желтым цветом
    }
    innerPixmap = basePixmap.copy(); // Копируем basePixmap в innerPixmap после изменений
}

void SecondWindow::drawRectanglePixmap()
{
    if (rectangle) {
        rectanglePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для rectanglePixmap
        QPainter painter(&rectanglePixmap);
        rectangle->draw(painter); // Рисуем прямоугольник внутри rectanglePixmap
    }
}

void SecondWindow::updateDrawing()
{
    innerPixmap = basePixmap.copy(); // Восстанавливаем изначальное состояние
    if (rectangleVisible) {
        QPainter painter(&innerPixmap);
        painter.drawPixmap(0, 0, rectanglePixmap); // Отображаем rectanglePixmap внутри innerPixmap
    }
}

void SecondWindow::restoreBasePixmap()
{
    basePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для basePixmap
    QPainter painter(&basePixmap);
    painter.setPen(Qt::black);
    painter.drawRect(basePixmap.rect().adjusted(0, 0, -1, -1)); // Рисуем границу внутреннего окна
    innerPixmap = basePixmap.copy(); // Восстанавливаем изначальное состояние
}
