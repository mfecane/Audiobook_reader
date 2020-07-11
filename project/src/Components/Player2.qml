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
            from: 0.0
            to: 1.0
            value: BackEnd.bookProgress
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: rootId.width - 40
            Layout.topMargin: 20
            Layout.rightMargin: 20
            Layout.leftMargin: 20
        }
        Label {
            Layout.alignment: Qt.AlignCenter
            text: BackEnd.currentTime
            color: "white"
            font.pointSize: 14
            font.weight: Font.DemiBold
        }
        RowLayout {
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
            spacing:20
            ImageButton {
                Layout.alignment:  Qt.AlignVCenter
                src: "qrc:/images/prev.png"
                onClicked: BackEnd.prev()
            }
            PlayButton {
                height:width
                onClicked: {
                    if(!isRecording) {
                        BackEnd.play()
                        isRecording = true
                    }
                    else {
                        BackEnd.stop()
                        isRecording = false
                    }
                }
            }
            ImageButton {
                Layout.alignment:  Qt.AlignVCenter
                src: "qrc:/images/next.png"
                onClicked: BackEnd.next()
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            ImageButton {
                src: "qrc:/images/jmp_back_x2.png"
                onClicked: BackEnd.jumpBack(60)
            }
            ImageButton {
                src: "qrc:/images/jmp_back.png"
                onClicked: BackEnd.jumpBack(10)
            }
            ImageButton {
                src: "qrc:/images/jmp_fwd.png"
                onClicked: BackEnd.jumpForeward(10)
            }
            ImageButton {
                src: "qrc:/images/jmp_fwd_x2.png"
                onClicked: BackEnd.jumpForeward(60)
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            ImageButton {
                src: "qrc:/images/minus.png"
                onClicked: BackEnd.decreaseTempo()
            }
            Label {
                text: BackEnd.tempo
                color: "white"
                font.pointSize: 18
                font.weight: Font.DemiBold
            }
            ImageButton {
                src: "qrc:/images/plus.png"
                onClicked: BackEnd.increaseTempo()
            }
        }

        ProgressBar {
            from: 0.0
            to: 1.0
            value: BackEnd.fileProgress
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: rootId.width-40
            Layout.bottomMargin: 20
            Layout.rightMargin: 20
            Layout.leftMargin: 20
        }
    }
    Button {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 50
        contentItem:
            Image {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            id: tabimg
            source: "qrc:/images/tab_button.png"
        }
        background :Item{}
    }
}
