import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import qyvlik.thread 0.1

Window {
    visible: true

    id: app

    Flow {
        anchors.fill: parent

        Button {
            text: "open file dialog"
            onClicked: {
                fileDialog.open()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        selectFolder: true
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl);
            connector.sendMessage(fileDialog.fileUrl.toString())
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }

    ThreadConnector {
        id: connector
        source: "./thread/thread_dir_size.qml"
        onMessageReceived: {
            console.log(message.toString())
        }
    }

}

