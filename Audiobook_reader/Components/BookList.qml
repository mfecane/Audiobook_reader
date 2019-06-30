import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12
import io.qt.examples.backend 1.0

ListView {
    id: booksView
    width: parent.width
    model:BackEnd.audioBookList
    delegate: ItemDelegate {
        width: parent.width
        text: model.text
        onClicked: booksView.currentIndex = index
    }
    ScrollBar.vertical: ScrollBar { }
    onCurrentItemChanged: BackEnd.setCurrentAudiobookIndex(currentIndex)
}
