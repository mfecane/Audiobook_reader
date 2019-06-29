import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import io.qt.examples.backend 1.0
import Qt.labs.folderlistmodel 2.12

Rectangle {
    color: "yellow"
    ListView {
        id: fileView
        anchors.fill: parent
        clip: true
        model: BackEnd.audioBookFileList
        //currentIndex: BackEnd.currentBookFile // TODO: fix this
        delegate: ItemDelegate {
            width: parent.width
            text: model.text
            onClicked: fileView.currentIndex = index
        }
        ScrollBar.vertical: ScrollBar { }
        onCurrentItemChanged: BackEnd.currentBookFile = currentIndex
    }
}
