import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import io.qt.examples.backend 1.0

Rectangle {
    id:rootId

    color: "green"
    ColumnLayout {
        anchors.fill:parent
        spacing:10

        ProgressBar {
            Layout.preferredWidth: rootId.width
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "magenta"
            Button{
                text: "Play"
                anchors.centerIn: parent
                visible: BackEnd.isPlaying ? false : true
                onClicked: BackEnd.play()
            }
            Button{
                text: "Stop"
                anchors.centerIn: parent
                visible: BackEnd.isPlaying ? true : false
                onClicked: BackEnd.stop()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Button {
                text:"prev"
                onClicked: BackEnd.prev()
            }
            Button {
                text:"<<"
                onClicked: BackEnd.jumpBack()
            }
            Button {
                text:"a"
                onClicked: BackEnd.speedUp()
            }
            Button {
                text:">>"
                onClicked: BackEnd.jumpForeward()
            }
            Button {
                text:"fwd"
                onClicked: BackEnd.next()
            }
        }

        ProgressBar {
            from: 0.0
            to: 1.0
            value: BackEnd.fileProgress
            Layout.preferredWidth: rootId.width
        }

//        Rectangle {
//            implicitHeight: 100;
//            implicitWidth: 100;
//            color: "orange"
//        }

//        Rectangle {
//            implicitHeight: 100;
//            implicitWidth: 100;
//            color: "orange"
//        }

//        Rectangle {
//            implicitHeight: 100;
//            implicitWidth: 100;
//            color: "orange"
//        }

//        Rectangle {
//            implicitHeight: 100;
//            implicitWidth: 100;
//            color: "orange"
//        }
    }
}
