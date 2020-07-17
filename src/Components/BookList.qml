import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12

import io.qt.examples.backend 1.0
import io.qt.examples.booklistmodel 1.0

import Theme 1.0

GridView {
    id: booksView
    //width: parent.width
    anchors.fill: parent
    anchors.margins: 25
    cellWidth: width/2
    cellHeight: 200


    model: BookListModel {
        id: bookListModel
        list: BackEnd.audioBookList
    }

    currentIndex: bookListModel.index

    Component {
        id: bookDelegate
        Item
        {
            id:wrapper
            height: booksView.cellHeight
            width: booksView.cellWidth
            property real progress: model.progress

            Button {
                anchors.fill: parent
                anchors.margins: 20
                state: (down | wrapper.GridView.isCurrentItem) ? "pressed" : hovered ? "hover" : "default"
                contentItem:
                    Label
                    {
                        leftPadding: 10
                        text: model.text
                        width: wrapper.width - 40
                        wrapMode: Label.WordWrap
                    }
                background: Rectangle
                {
                    id: backgroundRect
                    color: Theme.background_color_dark2
                    //color: wrapper.ListView.isCurrentItem ? "red" : "black"
                    Rectangle {
                        height:4
                        color: Theme.button_color
                        width: backgroundRect.width * wrapper.progress
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                    }
                } // Rectangle:backgroundRect
                onClicked: bookListModel.index = index
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
                    ] // states
                transitions: Transition {
                    ColorAnimation {
                        duration: 50
                    }
                }
            } // Button
        } //Item

//        Rectangle {
//            implicitHeight: 200
//            implicitWidth: booksView.width/2
//            Button {
//                anchors.margins: 10
//                anchors.fill: parent
//                id: wrapper

//                property real progress: model.progress

//                state: (wrapper.down | wrapper.ListView.isCurrentItem) ?
//                           "pressed" : wrapper.hovered ?
//                               "hover" : "default"
//                contentItem: Label {
//                    leftPadding: 10
//                    text: model.text
//                }

//                background:
//                    Rectangle {
//                    id:backgroundRect
//                    color: Theme.background_color
//                    Rectangle {
//                        height:4
//                        color: Theme.button_color
//                        width: backgroundRect.width * wrapper.progress
//                        anchors.left: parent.left
//                        anchors.bottom: parent.bottom
//                    }
//                }
//                states: [
//                    State {
//                        name : "hover"
//                        PropertyChanges {
//                            target: backgroundRect
//                            color: Theme.main_gray
//                        }
//                    },
//                    State {
//                        name : "pressed"
//                        PropertyChanges {
//                            target: backgroundRect
//                            color: Theme.main_gray
//                        }
//                    },
//                    State {
//                        name : "default"
//                        PropertyChanges {
//                            target: backgroundRect
//                            color: Theme.background_color
//                        }
//                    }
//                ] // states
//                transitions: Transition {
//                    ColorAnimation {
//                        duration: 50
//                    }
//                }
//                onClicked:
//                {
//                    booksView.currentIndex = index
//                    bookListModel.index = index
//                }
//            } // Button
//        } // Rectangle
    } // Component

    delegate: bookDelegate
    ScrollBar.vertical: ScrollBar { }
} // GridView
