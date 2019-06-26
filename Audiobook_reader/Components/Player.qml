import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import io.qt.examples.backend 1.0
import Qt.labs.folderlistmodel 2.12


ColumnLayout {
    id: playerLayout

    Layout.preferredWidth: 200
    Layout.minimumWidth: 200

    ProgressBar { width: parent.width }

    property bool isPlaying: BackEnd.isPlaying

    Item{
        width: parent.width
        height: 20
        Button{
            text: "Play"
            anchors.centerIn: parent
            visible: BackEnd.isPlaying ? false : true
            onClicked: BackEnd.play()
        }
        Button{
            text: "Stop"
            anchors.centerIn: parent
            visible: BackEnd.isPlaying ? true : false
            onClicked: BackEnd.stop()
        }
    }

    RowLayout {
        width: parent.width
        Button{
            width: parent.width/4
            text: "Back"
        }
        Button{
            width: parent.width/4
            text: "Jump Back"
        }
        Button{
            width: parent.width/4
            text: "Jump Forward"
        }
        Button {
            width: parent.width/4
            text: "Next"
        }
    }
    ProgressBar { width: parent.width }
    ListView {
        id:fileView
        clip: true
        width: playerLayout
        Layout.fillHeight: true
        FolderListModel {
            id: folderModel
            showDirs: false
            folder: BackEnd.currentFolderUrl
            nameFilters: ["*.mp3"]
        }
        model: folderModel
        delegate: ItemDelegate {
            text: model.fileName
            width: parent.width
            onClicked: fileView.currentIndex = index
        }
        ScrollBar.vertical: ScrollBar { }

        onCurrentItemChanged: BackEnd.setFileSlot(folderModel.get(currentIndex, "filePath"))
    }
}
