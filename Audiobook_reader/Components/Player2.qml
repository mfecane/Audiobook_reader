import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import io.qt.examples.backend 1.0
import Theme 1.0

Rectangle {
    id:rootId

    color: Theme.background_color
    ColumnLayout {
        anchors.fill:parent
        spacing:10

        ProgressBar {
            padding: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: rootId.width-40
        }
//        Rectangle {
//            Layout.alignment: Qt.AlignHCenter | Qt.AlignCenter
//            Layout.fillHeight: true
//            //Layout.fillWidth: true
//            color: "gold"
            RowLayout {
                //anchors.fill: parent
                Layout.preferredWidth: 250
                Layout.alignment: Qt.AlignVCenter
                Layout.fillHeight: true
                Button {
                    Layout.alignment:  Qt.AlignVCenter
                    contentItem: Image {
                        source: "qrc:/images/prev.png"
                    }
                    onClicked: BackEnd.prev()
                    background:
                        Item{ }
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "transparent"
                    Button{
                        text: "Play"
                        anchors.centerIn: parent
                        visible: BackEnd.isPlaying ? false : true
                        onClicked: BackEnd.play()
                        contentItem: Image {
                            source: "qrc:/images/play.png"
                        }
                        background:
                            Item{ }
                    }
                    Button{
                        text: "Stop"
                        anchors.centerIn: parent
                        visible: BackEnd.isPlaying ? true : false
                        onClicked: BackEnd.stop()
                        contentItem: Image {
                            source: "qrc:/images/play.png"
                        }
                        background:
                            Item{ }
                    }
                }
                Button {
                    Layout.alignment:  Qt.AlignVCenter
                    contentItem: Image {
                        source: "qrc:/images/next.png"
                    }
                    onClicked: BackEnd.next()
                    background:
                        Item{ }
                }
            }
//        }
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
            padding: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: rootId.width-40
        }
    }
}
