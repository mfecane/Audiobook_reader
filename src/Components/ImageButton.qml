import QtQuick 2.0
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.12

import Theme 1.0


Button {
    id:rootId

    property string src: ""

    background:
        Image {
        id: img
        source: src
        layer {
            id: color_effect
            enabled: true
            effect:
            ColorOverlay {
                anchors.fill: parent
                source: img
                color: Theme.button_color_hover
            }
        }
    }

    state: rootId.down ? "pressed" : rootId.hovered ? "hover" : "default"

    states: [
        State{
            name : "hover"
            PropertyChanges {
                target: color_effect
                enabled: true
            }
        },
        State{
            name : "pressed"
            PropertyChanges {
                target:color_effect
                enabled: true
            }
        },
        State{
            name : "default"
            PropertyChanges {
                target: color_effect
                enabled: false
            }
        }
    ]
//    transitions: Transition {
//        NumberAnimation  {
//            duration: 50
//        }
//    }
}
