#include "secondwindow.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include <iostream>

SecondWindow::SecondWindow(QWidget *parent, int width, int height)
    : QWidget(parent),
      rectangle(nullptr),
      isColored(false),
      clickCount(0),
      rectangleVisible(false),
      windowWidth(width),
      windowHeight(height)
{
//    setAttribute(Qt::WA_TranslucentBackground, true); // Устанавливаем прозрачный фон
    // Устанавливаем серый фон через палитру
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::green);
    setAutoFillBackground(true);
    setPalette(pal);

//    setStyleSheet("background-color: transparent;"); // Устанавливаем стиль прозрачного фона

    setMinimumSize(width, height); // Устанавливаем минимальный размер окна
    resize(width, height); // Устанавливаем начальный размер окна
    initializeImage();

    // Выводим размеры окна в консоль
    qDebug() << "Размер окна secondwindow при инициализации : " << windowWidth << "x" << windowHeight;
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
            drawBaseImage();
        } else if (clickCount == 2) {
            if (!rectangle) {
                rectangle = new Rectangle(0, 0, windowWidth * 2, windowHeight * 2); // Прямоугольник больше окна
//                rectangle = new Rectangle(0, 0, windowWidth * 0.2, windowHeight * 0.2); // Прямоугольник больше окна
                drawRectangleImage();
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
            restoreBaseImage();

            // Сообщаем QScrollArea о возврате к исходным размерам
            updateScrollArea();
        }
        update(); // Обновляем виджет для отрисовки
    }
}

void SecondWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // Создаем объект QPainter для рисования на виджете
    painter.drawImage(0, 0, innerImage); // Отображаем innerImage на виджете

    if (rectangleVisible && rectangle) {
        int centerX = (innerImage.width() - rectangleImage.width()) / 2;
        int centerY = (innerImage.height() - rectangleImage.height()) / 2;
        painter.drawImage(centerX, centerY, rectangleImage); // Отображаем rectangleImage на виджете по центру
    }
    Q_UNUSED(event);
}

void SecondWindow::initializeImage()
{
    innerImage = QImage(windowWidth, windowHeight, QImage::Format_ARGB32);
    innerImage.fill(Qt::transparent); // Устанавливаем прозрачный фон для innerImage
    baseImage = QImage(windowWidth, windowHeight, QImage::Format_ARGB32);
//    baseImage.fill(Qt::transparent); // Устанавливаем прозрачный фон для baseImage
    baseImage.fill(Qt::gray); // Устанавливаем прозрачный фон для baseImage
    drawBaseImage(); // Рисуем базовый innerImage
//    innerImage = baseImage.copy(); // Сохраняем базовое изображение в innerImage
}

void SecondWindow::drawBaseImage()
{
    std::cout << std::endl;
    std::cout << " In drawBaseImage :: " << std::endl;

    std::cout << "  SecWindow Width : " << this->width() << " SecWindow height: " << this->height();
    std::cout <<std::endl;
    std::cout << "  BaseImage width: " << baseImage.width() << " BaseImage height: " << baseImage.height();

//    baseImage.fill(Qt::transparent); // Устанавливаем прозрачный фон для baseImage
    baseImage.fill(Qt::gray); // Устанавливаем прозрачный фон для baseImage
//    QPainter painter(&baseImage);
    QPainter painter(&innerImage);
    if (isColored) {
        painter.fillRect(innerImage.rect().adjusted(1, 1, -1, -1), Qt::yellow); // Заливаем внутреннее окно желтым цветом
    }
    else {
        QPainter painter(&baseImage);
        painter.fillRect(baseImage.rect().adjusted(1, 1, -1, -1), Qt::gray); // Заливаем внутреннее окно желтым цветом

    }
//    innerImage = baseImage.copy(); // Копируем baseImage в innerImage после изменений
}

void SecondWindow::drawRectangleImage()
{
    if (rectangle) {
        rectangleImage = QImage(rectangle->width, rectangle->height, QImage::Format_ARGB32);
        rectangleImage.fill(Qt::transparent); // Устанавливаем прозрачный фон для rectangleImage
        QPainter painter(&rectangleImage);
        rectangle->draw(painter); // Рисуем прямоугольник внутри rectangleImage
    }
}

void SecondWindow::updateDrawing()
{
//    innerImage = baseImage.copy(); // Восстанавливаем изначальное состояние
//    if (rectangleVisible) {
//        QPainter painter(&innerImage);
//        int centerX = (innerImage.width() - rectangleImage.width()) / 2;
//        int centerY = (innerImage.height() - rectangleImage.height()) / 2;
////        painter.drawImage(centerX, centerY, rectangleImage); // Отображаем rectangleImage внутри innerImage по центру
//        painter.drawImage(0, 0, rectangleImage); // Отображаем rectangleImage внутри innerImage по центру

//    }
}

void SecondWindow::restoreBaseImage()
{
    std::cout << std::endl;
    std::cout << " In restoreBaseImage  :: " << std::endl;
    std::cout << " In restoreBaseImage SecWindow Width : " << this->width() << " SecWindow height: " << this->height();
    std::cout <<std::endl;
    std::cout << "BaseImage width: " << baseImage.width() << " BaseImage height: " << baseImage.height();

//    baseImage.fill(Qt::transparent); // Устанавливаем прозрачный фон для baseImage
//    baseImage.fill(Qt::gray); // Устанавливаем прозрачный фон для baseImage
//    QPainter painter(&baseImage);
    QPainter painter(&innerImage);
//    painter.setPen(Qt::black);
//    painter.drawRect(baseImage.rect().adjusted(0, 0, -1, -1)); // Рисуем границу внутреннего окна
//    innerImage = baseImage.copy(); // Восстанавливаем изначальное состояние
    painter.drawImage(0, 0 , baseImage);
    update();
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
