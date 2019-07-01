#include "audiobooklist.h"

AudioBookList::AudioBookList(QString root) :
    m_root(root)
{
    QDir d(m_root);
    if(d.exists()) {
        d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        d.setSorting(QDir::Name);
        list = d.entryInfoList();
        for(int i = 0; i < list.size(); ++i) {
            qDebug() << "adding audiobook folder" << list.at(i).absoluteFilePath() ;
            AudioBook* ab = AudioBook::createAudiobok(list.at(i).absoluteFilePath(), this);
            if(ab != nullptr) {
                qDebug() << "valid audiobook found";
                m_list.append(ab);
            }
        }
    }
}

QHash<int, QByteArray> AudioBookList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[ProgressRole] = "progress";
    return roles;
}

int AudioBookList::rowCount(const QModelIndex &parent) const {
    return m_list.size();
}

QVariant AudioBookList::data(const QModelIndex &index, int role) const {
    switch(role) {
    case TextRole:
        return m_list[index.row()]->folderName();
    case ProgressRole:
        return m_list[index.row()]->progress();
    }
    return false;
}

void AudioBookList::audioBookListItemChanged() {
    //    QModelIndex topLeft = createIndex(m_audiobook->index(), 0);
    //    QModelIndex bottomRight = createIndex(m_audiobook->index(), 0);
    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_list.size(), 0);
    QVector<int> roleVector;
    roleVector << AudiobookRoles::ProgressRole;
    emit dataChanged(topLeft, bottomRight, roleVector);
    // this would not work
}

//void QAbstractItemModel::dataChanged(
//  const QModelIndex &topLeft,
//  const QModelIndex &bottomRight,
//  const QVector<int> &roles = ...)

//void AudioBookList::playlistItemChanged() {
//    QModelIndex topLeft = createIndex(0, 0);
//    QModelIndex bottomRight = createIndex(m_list.size()-1, 0);
//    QVector<int> roleVector;
//    roleVector << AudiobookRoles::ProgressRole;
//    emit dataChanged(topLeft, bottomRight, roleVector);
//}
