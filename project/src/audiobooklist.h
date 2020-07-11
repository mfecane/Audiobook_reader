#pragma once
#include <QObject>
#include <QAbstractListModel>

#include "audiobook.h"

class AudioBookList : public QAbstractListModel {
    Q_OBJECT

public:

    enum AudiobookRoles {
        TextRole = Qt::UserRole +1,
        ProgressRole
    };

    AudioBookList(QString root);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setRootFolder(QString value) {
        m_root = value;
    }

    const AudioBook* at(int i) {
        return m_list.at(i);
    }

    void setIndex(int value) {
        m_index = value;
    }
    int getIndex() {
        return m_index;
    }

    //public slots:

    //    void playlistItemChanged();

public slots:

    void audioBookListItemChanged();

private:

    QString m_root;
    QFileInfoList list;
    QVector<AudioBook*> m_list;
    int m_index;

};