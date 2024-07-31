#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include "rectangle.h"

class SecondWindow : public QWidget
{
    Q_OBJECT

public:
    SecondWindow(QWidget *parent = nullptr, int width = 400, int height = 300);
    ~SecondWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int clickCount;
    bool isColored;
    Rectangle *rectangle;
    QPixmap pixmap;
    QRect innerRect;
    QPixmap innerPixmap; // Для хранения отрисованного изображения внутреннего виджета
    QPixmap basePixmap;  // Базовый pixmap для хранения начального состояния
    QPixmap rectanglePixmap; // Pixmap для хранения отрисованного прямоугольника
    bool rectangleVisible; // Флаг для отслеживания видимости прямоугольника

    void initializePixmap();
    void updateInnerRect();
    void drawBasePixmap();
    void drawRectanglePixmap();
    void updateDrawing();
    void restoreBasePixmap();
};

#endif // SECONDWINDOW_H
