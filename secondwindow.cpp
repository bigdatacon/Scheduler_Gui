#include "secondwindow.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>

SecondWindow::SecondWindow(QWidget *parent, int width, int height)
    : QWidget(parent),
      rectangle(nullptr),
      isColored(false),
      clickCount(0),
      rectangleVisible(false),
      windowWidth(width),
      windowHeight(height)
{
    setAttribute(Qt::WA_TranslucentBackground, true); // Устанавливаем прозрачный фон
    setStyleSheet("background-color: transparent;"); // Устанавливаем стиль прозрачного фона
    setMinimumSize(width, height); // Устанавливаем минимальный размер окна
    resize(width, height); // Устанавливаем начальный размер окна
    initializePixmap();

    // Выводим размеры окна в консоль
    qDebug() << "Размер окна: " << windowWidth << "x" << windowHeight;
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
            drawBasePixmap();
        } else if (clickCount == 2) {
            if (!rectangle) {
                rectangle = new Rectangle(0, 0, windowWidth * 2, windowHeight * 2); // Прямоугольник больше окна
//                rectangle = new Rectangle(0, 0, windowWidth * 0.2, windowHeight * 0.2); // Прямоугольник больше окна
                drawRectanglePixmap();
            }
            rectangleVisible = true;
            updateDrawing();

            // Сообщаем QScrollArea о новых размерах содержимого
            updateScrollArea();
        } else if (clickCount == 3) {
            isColored = false;
            rectangleVisible = false;
            delete rectangle;
            rectangle = nullptr;
            clickCount = 0;
            restoreBasePixmap();

            // Сообщаем QScrollArea о возврате к исходным размерам
            updateScrollArea();
        }
        update(); // Обновляем виджет для отрисовки
    }
}

void SecondWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // Создаем объект QPainter для рисования на виджете
    painter.drawPixmap(0, 0, innerPixmap); // Отображаем innerPixmap на виджете

//    if (rectangleVisible && rectangle) {
//        painter.drawPixmap(0, 0, rectanglePixmap); // Отображаем rectanglePixmap на виджете
//    }

    if (rectangleVisible && rectangle) {
        int centerX = (innerPixmap.width() - rectanglePixmap.width()) / 2;
        int centerY = (innerPixmap.height() - rectanglePixmap.height()) / 2;
        painter.drawPixmap(centerX, centerY, rectanglePixmap); // Отображаем rectanglePixmap на виджете по центру
    }

    Q_UNUSED(event);
}

void SecondWindow::initializePixmap()
{
    innerPixmap = QPixmap(windowWidth, windowHeight);
    innerPixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для innerPixmap
    basePixmap = QPixmap(windowWidth, windowHeight);
    basePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для basePixmap
    drawBasePixmap(); // Рисуем базовый innerPixmap
    innerPixmap = basePixmap.copy(); // Сохраняем базовое изображение в innerPixmap
}

void SecondWindow::drawBasePixmap()
{
    basePixmap.fill(Qt::transparent); // Устанавливаем прозрачный фон для basePixmap
    QPainter painter(&basePixmap);
    if (isColored) {
        painter.fillRect(basePixmap.rect().adjusted(1, 1, -1, -1), Qt::yellow); // Заливаем внутреннее окно желтым цветом
    }
    innerPixmap = basePixmap.copy(); // Копируем basePixmap в innerPixmap после изменений
}

void SecondWindow::drawRectanglePixmap()
{
    if (rectangle) {
        rectanglePixmap = QPixmap(rectangle->width, rectangle->height);
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
//        painter.drawPixmap(0, 0, rectanglePixmap); // Отображаем rectanglePixmap внутри innerPixmap

        int centerX = (innerPixmap.width() - rectanglePixmap.width()) / 2;
        int centerY = (innerPixmap.height() - rectanglePixmap.height()) / 2;
        painter.drawPixmap(centerX, centerY, rectanglePixmap); // Отображаем rectanglePixmap внутри innerPixmap по центру
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

void SecondWindow::updateScrollArea()
{
    if (parentWidget() && parentWidget()->parentWidget()) {
        QScrollArea *scrollArea = qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
        if (scrollArea) {
            if (rectangleVisible && rectangle) {
                scrollArea->widget()->setMinimumSize(rectangle->width, rectangle->height);
            } else {
                scrollArea->widget()->setMinimumSize(windowWidth, windowHeight);
            }
        }
    }
}
