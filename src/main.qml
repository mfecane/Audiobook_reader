import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import Qt.labs.settings 1.0
//import QtWinExtras 1.0


import io.qt.examples.backend 1.0
import Theme 1.0
import "Components"
import kh.components 1.0

BLWindow {
    id: window
    visible: true
    width: 640
    height: 480
    minimumWidth: 300
    minimumHeight: 640
    title: qsTr("Audiobook Reader")

    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    RowLayout {
        anchors.fill:parent
        spacing: 0
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "red"
            ColumnLayout {
                spacing: 0
                anchors.fill: parent
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Theme.background_color_dark
//                    Image { //Shadow
//                        anchors.top: parent.top
//                        anchors.bottom: parent.bottom
//                        anchors.right: parent.right
//                        fillMode: Image.Tile
//                        width: 37
//                        source: "qrc:/images/shadow.png"
//                    }
                    BookList {
                        id:booksPage
                        anchors.fill: parent
                        visible: true
                    }
                    Ass {
                        id:asspage
                        anchors.fill: parent
                        visible: false
                    }
                } // Rectangle
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    color: Theme.background_color_dark3
                    SettingsButton {
                        width:parent.width
                        onClicked: {
                            asspage.visible = !asspage.visible;
                            booksPage.visible = !booksPage.visible
                        }
                    }
                } // Rectangle
            } //ColumnLayout
        } // Rectangle
        Rectangle {
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            color: "blue"
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                Player2 {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 400
                }

                Playlist {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
