import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.1
import Theme 1.0

Button {
    id:rootId
    padding: 20
    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"
    contentItem:
        RowLayout {
        spacing: 5
        Image {
            source: "qrc:/images/gear.png"
        }
        Label {
            Layout.fillWidth: true
            text: "Settings"
            font.pointSize: 14
            color: Theme.main_gray
        }
    }
    background:
        Rectangle{
        id:backgroundRect
        color:"transparent"
        opacity: 0.2
    }
    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: backgroundRect
                color: Theme.main_gray
                border.color: Theme.main_gray
            }
        },
        State{
            name : "pressed"
            PropertyChanges {
                target: backgroundRect
                color: Theme.main_gray
                border.color: Theme.main_gray
            }
        },
        State{
            name : "default"
            PropertyChanges {
                target: backgroundRect
                color: "transparent"
                border.color: "transparent"
            }
        }
    ]
    transitions: Transition {
        ColorAnimation {
            duration: 50
        }
    }
}
