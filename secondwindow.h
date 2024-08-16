#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QImage>
#include "rectangle.h"


class Rectangle;

class SecondWindow : public QWidget
{
    Q_OBJECT

public:
    SecondWindow(QWidget *parent = nullptr, int width = 1400, int height = 800);
    ~SecondWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void initializeImage();
    void drawBaseImage();
    void drawRectangleImage();
    void updateDrawing();
    void restoreBaseImage();
    void updateScrollArea();

    Rectangle *rectangle;
    bool isColored;
    int clickCount;
    bool rectangleVisible;
    int windowWidth;
    int windowHeight;

    QImage baseImage;
    QImage innerImage;
    QImage rectangleImage;
};

#endif // SECONDWINDOW_H
