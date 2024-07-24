#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void drawBarChart(QPixmap &pixmap);
    void updateBarChart();

    QPixmap chartPixmap;
    QLabel *chartLabel;
    QVBoxLayout *mainLayout;

    struct Bar {
        QRect rect;
        QString label;
    };

    QVector<Bar> bars;
    Bar *selectedBar;
    QPoint lastMousePos;
};

#endif // MAINWINDOW_H
