#include "mainwindow.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      webView(new QWebEngineView(this)),
      selectedBar(nullptr),
      httpServer(new QTcpServer(this))
{
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(webView);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    drawBarChart();
    startHttpServer(); // Запуск веб-сервера
    webView->setUrl(QUrl("http://127.0.0.1:8080")); // Отображение страницы в приложении
    QDesktopServices::openUrl(QUrl("http://127.0.0.1:8080")); // Открытие страницы в браузере
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

void MainWindow::drawBarChart()
{
    int barWidth = 50;
    int spacing = 20;
    int y = 100;

    for (int i = 0; i < 5; ++i) {
        QRect rect(100, y, 200, barWidth);
        bars.append({rect, QString("Bar %1").arg(i + 1)});
        y += barWidth + spacing;
    }
}

void MainWindow::updateBarPosition()
{
    QString script = generateUpdateScript();
    webView->page()->runJavaScript(script);
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
        bool overlap = false;
        for (const Bar &otherBar : bars) {
            if (&bar != &otherBar && barsOverlap(bar.rect, otherBar.rect)) {
                overlap = true;
                break;
            }
        }
        jsonBar["color"] = overlap ? "red" : "blue";
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

QString MainWindow::generateUpdateScript()
{
    QJsonArray jsonBars;
    for (const Bar &bar : bars) {
        QJsonObject jsonBar;
        jsonBar["x"] = bar.rect.left();
        jsonBar["y"] = bar.rect.top();
        jsonBar["label"] = bar.label;
        bool overlap = false;
        for (const Bar &otherBar : bars) {
            if (&bar != &otherBar && barsOverlap(bar.rect, otherBar.rect)) {
                overlap = true;
                break;
            }
        }
        jsonBar["color"] = overlap ? "red" : "blue";
        jsonBars.append(jsonBar);
    }

    QJsonDocument doc(jsonBars);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    QString script = R"(
        let bars = )" + jsonString + R"(;
        bars.forEach(bar => {
            updateBar(bar);
        });
    )";

    return script;
}

void MainWindow::startHttpServer()
{
    connect(httpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);

    if (!httpServer->listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }
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
