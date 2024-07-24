#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      chartPixmap(800, 600),
      chartLabel(new QLabel(this)),
      selectedBar(nullptr)
{
    mainLayout = new QVBoxLayout;
    chartLabel->setPixmap(chartPixmap);
    mainLayout->addWidget(chartLabel);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    drawBarChart(chartPixmap);
    chartLabel->setPixmap(chartPixmap);
}

MainWindow::~MainWindow()
{
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    for (Bar &bar : bars) {
        if (bar.rect.contains(lastMousePos)) {
            selectedBar = &bar;
            break;
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (selectedBar) {
        int dy = event->pos().y() - lastMousePos.y();
        selectedBar->rect.moveTop(selectedBar->rect.top() + dy);
        lastMousePos = event->pos();
        updateBarChart();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    selectedBar = nullptr;
}

void MainWindow::drawBarChart(QPixmap &pixmap)
{
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw X and Y axes
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(50, 50, 50, pixmap.height() - 50); // Y axis
    painter.drawLine(50, pixmap.height() - 50, pixmap.width() - 50, pixmap.height() - 50); // X axis

    int barWidth = 50;
    int spacing = 20;
    int y = 100;

    for (int i = 0; i < 5; ++i) {
        QRect rect(100, y, 200, barWidth);
        bars.append({rect, QString("Bar %1").arg(i + 1)});
        y += barWidth + spacing;
    }

    for (const Bar &bar : bars) {
        painter.setBrush(Qt::blue);
        painter.drawRect(bar.rect);
        painter.drawText(bar.rect, Qt::AlignCenter, bar.label);
    }
}

void MainWindow::updateBarChart()
{
    QPixmap newPixmap = chartPixmap; // Копируем текущий график
    newPixmap.fill(Qt::white);
    QPainter painter(&newPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw X and Y axes
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(50, 50, 50, newPixmap.height() - 50); // Y axis
    painter.drawLine(50, newPixmap.height() - 50, newPixmap.width() - 50, newPixmap.height() - 50); // X axis

    for (const Bar &bar : bars) {
        bool overlap = false;
        for (const Bar &otherBar : bars) {
            if (&bar != &otherBar && barsOverlap(bar.rect, otherBar.rect)) {
                overlap = true;
                break;
            }
        }
        if (overlap) {
            painter.setBrush(Qt::red);
        } else {
            painter.setBrush(Qt::blue);
        }
        painter.drawRect(bar.rect);
        painter.drawText(bar.rect, Qt::AlignCenter, bar.label);
    }

    chartLabel->setPixmap(newPixmap);
}

bool MainWindow::barsOverlap(const QRect &rect1, const QRect &rect2)
{
    return rect1.intersects(rect2);
}
