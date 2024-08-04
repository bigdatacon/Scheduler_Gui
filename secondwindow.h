#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QScrollArea>
#include "rectangle.h"

class SecondWindow : public QWidget
{
    Q_OBJECT

public:
    SecondWindow(QWidget *parent = nullptr, int width = 800, int height = 600);
    ~SecondWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initializePixmap();
    void drawBasePixmap();
    void drawRectanglePixmap();
    void updateDrawing();
    void restoreBasePixmap();
    void updateScrollArea();

    QPixmap innerPixmap;
    QPixmap basePixmap;
    QPixmap rectanglePixmap;

    Rectangle *rectangle;
    bool rectangleVisible;
    bool isColored;
    int clickCount;

    int windowWidth;
    int windowHeight;
};

#endif // SECONDWINDOW_H
