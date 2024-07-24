#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void drawBarChart(QPixmap &pixmap);
    void updateBarChart();
    void updateBarPosition();
    bool barsOverlap(const QRect &rect1, const QRect &rect2);
    QString generateHtmlChart();
    QString generateUpdateScript(const QRect &rect, const QString &color);
    void startHttpServer();
    bool isPortInUse(int port);

    QWebEngineView *webView;
    QVBoxLayout *mainLayout;
    QTcpServer *httpServer;

    QPixmap chartPixmap;
    QLabel *chartLabel;

    struct Bar {
        QRect rect;
        QString label;
    };

    QVector<Bar> bars;
    Bar *selectedBar;
    QPoint lastMousePos;

private slots:
    void handleNewConnection();
    void readClient();
};

#endif // MAINWINDOW_H
