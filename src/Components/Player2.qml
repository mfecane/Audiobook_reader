import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import io.qt.examples.backend 1.0
import Theme 1.0

Rectangle {
    id:rootId

    color: Theme.background_color_dark

    Image {
        width: rootId.width
        height: rootId.height
        source: "qrc:/images/back.png"
    }

    RowLayout {
        anchors.right: parent.right
        anchors.top: parent.top
        Layout.alignment: Qt.AlignRight
        spacing: 0
        AppButton {
            Layout.preferredWidth: 46
            Layout.preferredHeight: 30
            src:"qrc:/images/collapse.png"
            onClicked: window.hide()
            }
        AppButton {
            Layout.preferredWidth: 46
            Layout.preferredHeight: 30
            src:"qrc:/images/full.png"
            onClicked: window.showMaximized()
            }
        AppButton {
            Layout.preferredWidth: 46
            Layout.preferredHeight: 30
            hoverColor: Theme.close_hover
            src:"qrc:/images/close.png"
            onClicked: {
                    window.close()
                }
            }
    } // RowLayout

    ColumnLayout {
        anchors.fill:parent
        spacing:10


        Label { // Title
            Layout.alignment: Qt.AlignHCenter
            text: BackEnd.audioBookName
            color: Theme.accent2
            font.pixelSize: 20
            font.weight: Font.DemiBold
            Layout.topMargin: 50
            Layout.rightMargin: 20
            Layout.leftMargin: 20
        }
        ProgressBar {
            from: 0.0
            to: 1.0
            value: BackEnd.bookProgress
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: rootId.width - 40
            Layout.rightMargin: 20
            Layout.leftMargin: 20
        }
        Label {
            Layout.alignment: Qt.AlignCenter
            text: BackEnd.currentTime
            color: Theme.accent2
            font.pixelSize: 20
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
            Layout.preferredHeight: 32
            Layout.alignment: Qt.AlignCenter
            spacing:0
            ImageButton {
                height: 32
                width: 48
                src: "qrc:/images/minus.png"
                onClicked: BackEnd.decreaseTempo()
            }
            Item {
                Layout.preferredWidth: 42
                height: 32
                Label {
                    anchors.centerIn: parent
                    text: BackEnd.tempo
                    color: Theme.accent2
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                }
            }
            ImageButton {
                height: 32
                width: 48
                src: "qrc:/images/plus.png"
                onClicked: BackEnd.increaseTempo()
            }
        } // RowLayout

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
    } // ColumnLayout
//    Button {
//        anchors.left: parent.left
//        anchors.verticalCenter: parent.verticalCenter
//        anchors.verticalCenterOffset: 50
//        contentItem:
//            Image {
//            anchors.left: parent.left
//            anchors.verticalCenter: parent.verticalCenter
//            id: tabimg
//            source: "qrc:/images/tab_button.png"
//        }
//        background :Item{}
//    }
} // Rectangle
