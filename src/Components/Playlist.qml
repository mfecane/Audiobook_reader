import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Qt.labs.folderlistmodel 2.12
import io.qt.examples.backend 1.0
import io.qt.examples.audiobookmodel 1.0
import Theme 1.0

Rectangle {
    color: Theme.background_dark

    ListView {
        id: playlistView
        anchors.fill: parent
        spacing: 0
        clip: true

        model: AudioBookModel {
           id: audioBookModel
           audioBook: BackEnd.audioBook
        }

        currentIndex: audioBookModel.index

        Component {
            id: playlistDelegate
            Item {
                id: wrapper
                height: 58
                width: playlistView.width

                Button {
                    anchors.topMargin: 10
                    anchors.leftMargin: 15
                    anchors.rightMargin: 15
                    anchors.bottomMargin: 10
                    anchors.fill: parent

                    state: (down | wrapper.ListView.isCurrentItem) ?    "pressed" :
                            hovered ?                                   "hover" :
                            playlistView.currentIndex > index ?         "done" :
                                                                        "default"

                    contentItem: Label {
                        leftPadding: 10
                        font.pixelSize: 16
                        text: model.text
                        color: "white"
                    }

                    background:
                        Rectangle {
                        id:backgroundRect
                        color: Theme.background_color
                        } // Rectangle:backgroundRect
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
                        },
                        State {
                            name : "done"
                            PropertyChanges {
                                target: backgroundRect
                                color: Theme.main_derk_gray
                            }
                        }
                    ] // states
                    transitions: Transition {
                        ColorAnimation {
                            duration: 50
                        }
                    }
                    onClicked:
                        audioBookModel.index = index
                } //Button
            } //Rectangle
        } //Component

        delegate: playlistDelegate
        ScrollBar.vertical: ScrollBar { }
    }
}
