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
            body { font-family: Arial, sans-serif; position: relative; }
            .bar { position: absolute; cursor: pointer; }
            .axis { position: absolute; background-color: black; }
            .label { position: absolute; }
        </style>
    </head>
    <body>
        <script>
            function drawChart(bars) {
                document.body.innerHTML = '';

                // Draw Y axis
                let yAxis = document.createElement('div');
                yAxis.className = 'axis';
                yAxis.style.width = '2px';
                yAxis.style.height = '600px';
                yAxis.style.left = '50px';
                yAxis.style.top = '50px';
                document.body.appendChild(yAxis);

                // Draw X axis
                let xAxis = document.createElement('div');
                xAxis.className = 'axis';
                xAxis.style.width = '700px';
                xAxis.style.height = '2px';
                xAxis.style.left = '50px';
                xAxis.style.top = '650px';
                document.body.appendChild(xAxis);

                // Draw X axis labels
                for (let i = 0; i <= 700; i += 50) {
                    let xLabel = document.createElement('div');
                    xLabel.className = 'label';
                    xLabel.style.left = (50 + i - 10) + 'px';
                    xLabel.style.top = '670px';
                    xLabel.innerText = i / 10;
                    document.body.appendChild(xLabel);
                }

                // Draw Y axis labels
                for (let i = 0; i <= 600; i += 50) {
                    let yLabel = document.createElement('div');
                    yLabel.className = 'label';
                    yLabel.style.left = '20px';
                    yLabel.style.top = (650 - i - 10) + 'px';
                    yLabel.innerText = i / 10;
                    document.body.appendChild(yLabel);
                }

                bars.forEach(bar => {
                    let div = document.createElement('div');
                    div.className = 'bar';
                    div.style.left = (bar.x + 50) + 'px';
                    div.style.top = (bar.y + 50) + 'px';
                    div.style.width = bar.width + 'px';
                    div.style.height = bar.height + 'px';
                    div.style.backgroundColor = bar.color;
                    div.innerText = bar.label;
                    div.style.color = 'white';
                    div.style.textAlign = 'center';
                    div.style.lineHeight = bar.height + 'px';
                    div.setAttribute('data-label', bar.label);
                    document.body.appendChild(div);

                    div.onmousedown = (e) => {
                        let shiftX = e.clientX - div.getBoundingClientRect().left;
                        let shiftY = e.clientY - div.getBoundingClientRect().top;

                        const moveAt = (pageX, pageY) => {
                            div.style.left = pageX - shiftX + 'px';
                            div.style.top = pageY - shiftY + 'px';
                            updateOverlap();
                        };

                        const onMouseMove = (e) => {
                            moveAt(e.pageX, e.pageY);
                        };

                        document.addEventListener('mousemove', onMouseMove);

                        div.onmouseup = () => {
                            document.removeEventListener('mousemove', onMouseMove);
                            div.onmouseup = null;
                            // Send updated position to the server
                            let bar = {
                                x: parseInt(div.style.left) - 50,
                                y: parseInt(div.style.top) - 50,
                                width: parseInt(div.style.width),
                                height: parseInt(div.style.height),
                                label: div.getAttribute('data-label'),
                                color: div.style.backgroundColor
                            };
                            fetch('/update', {
                                method: 'POST',
                                headers: {
                                    'Content-Type': 'application/json'
                                },
                                body: JSON.stringify(bar)
                            });
                        };
                    };

                    div.ondragstart = () => {
                        return false;
                    };
                });
            }

            let bars = )" + jsonString + R"(;
            drawChart(bars.bars);

            function updateBar(bar) {
                let div = document.querySelector('.bar[data-label="' + bar.label + '"]');
                if (div) {
                    div.style.left = (bar.x + 50) + 'px';
                    div.style.top = (bar.y + 50) + 'px';
                    div.style.backgroundColor = bar.color;
                }
            }

            function updateOverlap() {
                let barDivs = document.querySelectorAll('.bar');
                barDivs.forEach(div1 => {
                    let rect1 = div1.getBoundingClientRect();
                    let overlap = false;
                    barDivs.forEach(div2 => {
                        if (div1 !== div2) {
                            let rect2 = div2.getBoundingClientRect();
                            if (rect1.left < rect2.right &&
                                rect1.right > rect2.left &&
                                rect1.top < rect2.bottom &&
                                rect1.bottom > rect2.top) {
                                overlap = true;
                            }
                        }
                    });
                    div1.style.backgroundColor = overlap ? 'red' : 'blue';
                });
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
        let div = document.querySelector('.bar[data-label="' + bar.label + '"]');
        if (div) {
            div.style.left = (bar.x + 50) + 'px';
            div.style.top = (bar.y + 50) + 'px';
            div.style.backgroundColor = bar.color;
        }
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
        } else if (line.startsWith("POST /update")) {
            while (clientSocket->canReadLine()) {
                QString body = clientSocket->readLine();
                QJsonDocument doc = QJsonDocument::fromJson(body.toUtf8());
                QJsonObject obj = doc.object();
                // Обновите данные бара в приложении на основе obj
                // Здесь вы можете обновить позиции баров и другие параметры
            }
        }
    }
}
