import QtQuick 2.12
import QtCharts 2.0
import QtQuick.Controls 2.12

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    Column {
        anchors.centerIn: parent

        Text {
            id: cpuUsageLabel
            text: "CPU Usage: " + cpuUsage.toFixed(2) + "%"
            font.pixelSize: 20
        }

        ChartView {
            width: 600
            height: 300
            theme: ChartView.ChartThemeDark
            antialiasing: true

            LineSeries {
                id: cpuSeries
                name: "CPU Usage"
                axisX: ValueAxis { min: 0; max: 60 }
                axisY: ValueAxis { min: 0; max: 100 }

                Component.onCompleted: {
                    cpuUsageData = [];
                }

                function appendData(usage) {
                    cpuUsageData.push(usage);
                    if (cpuUsageData.length > 60) {
                        cpuUsageData.shift();
                    }
                    cpuSeries.clear();
                    for (var i = 0; i < cpuUsageData.length; i++) {
                        cpuSeries.append(i, cpuUsageData[i]);
                    }
                }
            }
        }
    }

    Connections {
        target: processManager
        onCpuUsageChanged: {
            var usage = processManager.getCpuUsage();
            cpuUsageLabel.text = "CPU Usage: " + usage.toFixed(2) + "%";
            cpuSeries.appendData(usage);
        }
    }
}
