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

    void updateInnerRect();

#endif // SECONDWINDOW_H

    void drawInnerPixmap();
};
