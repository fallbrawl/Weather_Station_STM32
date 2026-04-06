import QtQuick
import QtGraphs
import QtQuick.Controls

Window {
    width: 900
    height: 500
    visible: true
    title: "ESP32 Weather Station"
    color: "#1e1e1e"

    Row {
        id: header
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 40
        topPadding: 20

        Label { 
            text: "TEMP: " + weatherBackend.temperature.toFixed(2) + "°C"
            color: "#ff5555"; font.pixelSize: 28 
        }

        Rectangle {
            width: 2
            height: 50
            color: "#444444"
            anchors.verticalCenter: parent.verticalCenter
        }

        Label { 
            text: "HUM: " + weatherBackend.humidity.toFixed(2) + "%"
            color: "#55ff55"; font.pixelSize: 28 
        }

    }

    GraphsView {
        id: chartView

        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.margins: 40

        axisX: ValueAxis { id: xAxis; min: 0; max: 50 }
        axisY: ValueAxis { id: yAxis; min: 0; max: 40 }

        LineSeries {
            id: tempSeries
            name: "Temperature"
            color: "#ff5555"
            axisX: xAxis
            axisY: yAxis
        }

        Connections {
            target: weatherBackend
            function onDataChanged() {
                appendData(weatherBackend.temperature)
            }
        }

    }

    property int tick: 0
    function appendData(t) {
        tempSeries.append(tick, t)
        tick++
        
        if (tick > 50) {
            xAxis.min++;
            xAxis.max++;
        }
    }

}