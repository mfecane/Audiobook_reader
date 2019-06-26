import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import io.qt.examples.backend 1.0

Rectangle {
    Flickable {
        width: parent.width
        anchors.fill: parent
        contentHeight: child.height
        ColumnLayout {
            id: child
            Label {
                Layout.fillWidth: true
                text: "Root path"
                font.pointSize: 14
            }
            Label {
                Layout.fillWidth: true
                text: "Specify root path of all your audiobooks"
                font.pointSize: 10
                wrapMode: Text.WordWrap
            }
            Text {
                Layout.fillWidth: true
                id: fileLabel
                text: BackEnd.rootPath
                font.pointSize: 14
                elide: Text.ElideLeft
                MouseArea{
                    anchors.fill:parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: Qt.openUrlExternally(BackEnd.rootPathUrl)
                }
            }
            Button {
                text: "Browse"
                onClicked: openFolderDialog.open()
            }
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
        }
        ScrollBar.vertical: ScrollBar { }
    }

    OpenFolderDialog {
        id: openFolderDialog
        folder: BackEnd.rootPathUrl
        onAccepted: BackEnd.rootPathUrl = openFolderDialog.folder
    }
}
