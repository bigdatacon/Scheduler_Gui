import QtQuick 2.15
import QtQuick.Controls 2.15


ApplicationWindow {
    visible: true
    width: 800
    height: 600

    CustomPlot {
        id: customPlot
        anchors.fill: parent
        xAxis.range: Qt.point(0, 10)
        yAxis.range: Qt.point(0, 10)
    }
}
