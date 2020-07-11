import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true
    Flickable{
        anchors.fill: parent

        clip: true
        contentHeight: child.height + 80

        Item {
            width: parent.width
            ColumnLayout {
                id: child
                anchors.margins: 40
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width - 80
                spacing: 10
                Label {
                    Layout.fillWidth: true
                    text: "Folder path"
                    font.pointSize: 14
                }
                Label {
                    Layout.fillWidth: true
                    text: "Select folder to store recorded files."
                    font.pointSize: 10
                    wrapMode: Text.WordWrap
                }
                Text {
                    Layout.fillWidth: true
                    id:fileLabel
                    //text: BackEnd.filePath
                    font.pointSize: 14
                    elide: Text.ElideMiddle
                    //        MouseArea{
                    //            anchors.fill:parent
                    //            cursorShape: Qt.PointingHandCursor
                    //            onClicked: Qt.openUrlExternally(BackEnd.fileUrl)
                    //        }
                }
                Button {
                    text: "Browse"
                    //                onClicked: openFolderDialog.open()
                    //                enabled: BackEnd.lockParam
                }
            }
        }
    }
}
