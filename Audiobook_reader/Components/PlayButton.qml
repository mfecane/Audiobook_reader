import QtQuick 2.0
import QtQuick.Controls 2.1
import Theme 1.0

Button {
    implicitWidth: 115
    implicitHeight: implicitWidth
    id: rootId

    property bool isRecording: false

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    contentItem:
        Rectangle {
        implicitWidth:rootId.width
        implicitHeight:rootId.height
        color: "transparent"

        Image {
            anchors.centerIn: parent
            source: !rootId.isRecording ? "qrc:/images/play.png" : "qrc:/images/pause.png"
        }
    }

    background: Rectangle {
        id:backgroundRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width:rootId.width
        height: width
        color: Theme.button_color
        radius: width * 0.5
    }

    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_color_hover
            }
        },
        State{
            name : "pressed"
            PropertyChanges {
                target: backgroundRect
                color: "white"
            }
        },
        State{
            name : "default"
            PropertyChanges {
                target: backgroundRect
                color: Theme.button_color
            }
        }
    ]
    transitions: Transition {
        ColorAnimation {
            duration: 50
        }
    }

}
