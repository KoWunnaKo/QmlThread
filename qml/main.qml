import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Core 0.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: {
                    Qt.quit();
                    new Date().toJSON();
                    DataView ;
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 100
        TextField {
            id: input
            Layout.fillWidth: true
            text: "15"
        }
        Button {
            text: qsTr("Start")
            Layout.fillWidth: true
            onClicked: {
                thread.sendMessageToWorker({"OK":input.text})
            }
        }
        Text {
            id: info
        }
    }

    FooHelper {
        id: foo
    }

    Thread {
        id: thread
        source: "./worker.js"

        // 帮助对象列表
        helpers: [foo ]

        onMessage: {
            console.log(messageObject.OK)
            info.text = "result:" + messageObject.OK;
        }
    }
}

