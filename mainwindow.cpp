#include "mainwindow.h"
#include <QPainter>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QDesktopServices>
#include <QUrl>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // для close

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      webView(new QWebEngineView(this)),
      chartPixmap(800, 600),
      chartLabel(new QLabel(this)),
      selectedBar(nullptr),
      httpServer(new QTcpServer(this))
{
    mainLayout = new QVBoxLayout;
    chartLabel->setPixmap(chartPixmap);
    mainLayout->addWidget(chartLabel);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    drawBarChart(chartPixmap);
    chartLabel->setPixmap(chartPixmap);

    startHttpServer(); // Запуск веб-сервера
}

MainWindow::~MainWindow()
{
    httpServer->close();
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
        int dx = event->pos().x() - lastMousePos.x();
        int dy = event->pos().y() - lastMousePos.y();
        selectedBar->rect.moveLeft(selectedBar->rect.left() + dx);
        selectedBar->rect.moveTop(selectedBar->rect.top() + dy);
        lastMousePos = event->pos();
        updateBarPosition();
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

    // Draw X axis labels
    int xLabelInterval = 50;
    for (int i = 50; i < pixmap.width() - 50; i += xLabelInterval) {
        painter.drawLine(i, pixmap.height() - 55, i, pixmap.height() - 45);
        painter.drawText(i - 10, pixmap.height() - 30, QString::number((i - 50) / 10));
    }

    // Draw Y axis labels
    int yLabelInterval = 50;
    for (int i = pixmap.height() - 50; i > 50; i -= yLabelInterval) {
        painter.drawLine(45, i, 55, i);
        painter.drawText(20, i + 5, QString::number((pixmap.height() - 50 - i) / 10));
    }

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

    // Draw X axis labels
    int xLabelInterval = 50;
    for (int i = 50; i < newPixmap.width() - 50; i += xLabelInterval) {
        painter.drawLine(i, newPixmap.height() - 55, i, newPixmap.height() - 45);
        painter.drawText(i - 10, newPixmap.height() - 30, QString::number((i - 50) / 10));
    }

    // Draw Y axis labels
    int yLabelInterval = 50;
    for (int i = newPixmap.height() - 50; i > 50; i -= yLabelInterval) {
        painter.drawLine(45, i, 55, i);
        painter.drawText(20, i + 5, QString::number((newPixmap.height() - 50 - i) / 10));
    }

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

void MainWindow::updateBarPosition()
{
    if (selectedBar) {
        bool overlap = false;
        for (const Bar &otherBar : bars) {
            if (selectedBar != &otherBar && barsOverlap(selectedBar->rect, otherBar.rect)) {
                overlap = true;
                break;
            }
        }
        QString color = overlap ? "red" : "blue";
        QString script = generateUpdateScript(selectedBar->rect, color);
        webView->page()->runJavaScript(script);
        updateBarChart();
    }
}

bool MainWindow::barsOverlap(const QRect &rect1, const QRect &rect2)
{
    return rect1.intersects(rect2);
}

QString MainWindow::generateHtmlChart()
{
    QJsonArray jsonBars;
    for (const Bar &bar : bars) {
        QJsonObject jsonBar;
        jsonBar["x"] = bar.rect.left();
        jsonBar["y"] = bar.rect.top();
        jsonBar["width"] = bar.rect.width();
        jsonBar["height"] = bar.rect.height();
        jsonBar["label"] = bar.label;
        jsonBar["color"] = "blue";
        jsonBars.append(jsonBar);
    }

    QJsonObject json;
    json["bars"] = jsonBars;

    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    QString html = R"(
    <!DOCTYPE html>
    <html>
    <head>
        <title>Bar Chart</title>
        <style>
            body { font-family: Arial, sans-serif; }
            .bar { position: absolute; }
        </style>
    </head>
    <body>
        <script>
            function drawChart(bars) {
                document.body.innerHTML = '';
                bars.forEach(bar => {
                    let div = document.createElement('div');
                    div.className = 'bar';
                    div.style.left = bar.x + 'px';
                    div.style.top = bar.y + 'px';
                    div.style.width = bar.width + 'px';
                    div.style.height = bar.height + 'px';
                    div.style.backgroundColor = bar.color;
                    div.innerText = bar.label;
                    div.style.color = 'white';
                    div.style.textAlign = 'center';
                    div.style.lineHeight = bar.height + 'px';
                    div.setAttribute('data-label', bar.label);
                    document.body.appendChild(div);
                });
            }
            let bars = )" + jsonString + R"(;
            drawChart(bars.bars);

            function updateBar(bar) {
                let div = document.querySelector('.bar[data-label="' + bar.label + '"]');
                if (div) {
                    div.style.left = bar.x + 'px';
                    div.style.top = bar.y + 'px';
                    div.style.backgroundColor = bar.color;
                }
            }
        </script>
    </body>
    </html>
    )";

    return html;
}

QString MainWindow::generateUpdateScript(const QRect &rect, const QString &color)
{
    QJsonObject jsonBar;
    jsonBar["x"] = rect.left();
    jsonBar["y"] = rect.top();
    jsonBar["width"] = rect.width();
    jsonBar["height"] = rect.height();
    jsonBar["color"] = color;

    QJsonDocument doc(jsonBar);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    QString script = R"(
        let bar = )" + jsonString + R"(;
        updateBar(bar);
    )";

    return script;
}

void MainWindow::startHttpServer()
{
    int port = 8000; // Или другой порт, который вы хотите использовать
    if (isPortInUse(port)) {
        qDebug() << "Port" << port << "is in use. Server could not start!";
        return;
    }

    connect(httpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);

    if (!httpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port" << port << "!";
        QDesktopServices::openUrl(QUrl("http://127.0.0.1:" + QString::number(port)));
    }
}

bool MainWindow::isPortInUse(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return true; // Если не удалось создать сокет, считаем, что порт занят
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int result = ::bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    ::close(sock);

    return result < 0; // Если bind не удался, значит порт занят
}

void MainWindow::handleNewConnection()
{
    QTcpSocket *clientSocket = httpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::readClient);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void MainWindow::readClient()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    while (clientSocket->canReadLine()) {
        QString line = clientSocket->readLine();
        if (line.startsWith("GET")) {
            QString html = generateHtmlChart();
            QByteArray response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html; charset=\"utf-8\"\r\n";
            response += "Content-Length: " + QByteArray::number(html.size()) + "\r\n";
            response += "\r\n";
            response += html.toUtf8();
            clientSocket->write(response);
            clientSocket->disconnectFromHost();
            break;
        }
    }
}
