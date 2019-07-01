import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Qt.labs.folderlistmodel 2.12
import io.qt.examples.backend 1.0
import Theme 1.0

Rectangle {
    color: Theme.background_dark
    ListView {
        id: playlistView
        anchors.fill: parent
        clip: true
        model: BackEnd.playlist
        currentIndex: BackEnd.playlistIndex // TODO: fix this
        spacing: 0

        Component {
            id: contactsDelegate
            Rectangle {
                height: 58
                width: parent.width
                color:"transparent"
                Button {
                    anchors.topMargin: 10
                    anchors.leftMargin: 15
                    anchors.rightMargin: 15
                    anchors.bottomMargin: 10
                    anchors.fill:parent
                    id: wrapper

                    property real progress: model.progress;

                    state: (wrapper.down | wrapper.ListView.isCurrentItem) ?
                               "pressed" : wrapper.hovered ?
                                   "hover" : "default"
                    contentItem: Label {
                        leftPadding: 10
                        text: model.text
                        color: "white"
                    }

                    background:
                        Rectangle {
                        id:backgroundRect
                        color: Theme.background_color
                        Rectangle {
                            height:4
                            color: Theme.button_color
                            width: backgroundRect.width * progress
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                        }
                    }
                    states: [
                        State {
                            name : "hover"
                            PropertyChanges {
                                target: backgroundRect
                                color: Theme.button_color_hover
                            }
                        },
                        State {
                            name : "pressed"
                            PropertyChanges {
                                target: backgroundRect
                                color: Theme.main_gray
                            }
                        },
                        State {
                            name : "default"
                            PropertyChanges {
                                target: backgroundRect
                                color: Theme.background_color
                            }
                        }
                    ]
                    transitions: Transition {
                        ColorAnimation {
                            duration: 50
                        }
                    }
                    onClicked: playlistView.currentIndex = index
                }
            }
        }

        delegate: contactsDelegate
        ScrollBar.vertical: ScrollBar { }
        onCurrentItemChanged: BackEnd.playlistIndex = currentIndex
    }
}
