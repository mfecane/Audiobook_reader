import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12

import io.qt.examples.backend 1.0
import Theme 1.0

GridView {
    id: booksView
    width: parent.width
    model:BackEnd.audioBookList
    currentIndex: BackEnd.audioBookListIndex
    cellWidth: width/2
    cellHeight: 100

    anchors.fill: parent

    Component {
        id:bookDelegate
        Rectangle {
            implicitHeight: 100
            implicitWidth: booksView.width/2
            Button {
                anchors.margins: 10
                anchors.fill: parent
                id: wrapper

                property real progress: model.progress

                state: (wrapper.down | wrapper.ListView.isCurrentItem) ?
                           "pressed" : wrapper.hovered ?
                               "hover" : "default"
                contentItem: Label {
                    leftPadding: 10
                    text: model.text
                }

                background:
                    Rectangle {
                    id:backgroundRect
                    color: Theme.background_color
                    Rectangle {
                        height:4
                        color: Theme.button_color
                        width: backgroundRect.width * wrapper.progress
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                    }
                }
                states: [
                    State {
                        name : "hover"
                        PropertyChanges {
                            target: backgroundRect
                            color: Theme.main_gray
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
                onClicked: booksView.currentIndex = index
            }
        }
    }

    delegate: bookDelegate
    ScrollBar.vertical: ScrollBar { }

    onCurrentItemChanged: {
        console.log("item changed");
        BackEnd.audioBookListIndex = currentIndex;
    }
}
