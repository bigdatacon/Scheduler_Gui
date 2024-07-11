#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include "barchartview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BarChartView view;
    view.setRenderHint(QPainter::Antialiasing);

    QString filePath = QCoreApplication::applicationDirPath() + "/data.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open" << filePath;
        return -1;
    }

    QByteArray jsonData = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (document.isNull() || !document.isObject()) {
        qWarning() << "Invalid JSON data in" << filePath;
        return -1;
    }

    QJsonObject jsonObject = document.object();
    if (jsonObject.contains("js_operations") && jsonObject["js_operations"].isArray()) {
        QJsonArray operations = jsonObject["js_operations"].toArray();
        qDebug() << "Loaded js_operations with" << operations.size() << "entries";
        view.loadData(operations);
    } else {
        qWarning() << "No js_operations array found in" << filePath;
    }

    view.resize(800, 600);
    view.show();

    return a.exec();
}
