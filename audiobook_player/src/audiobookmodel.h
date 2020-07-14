#pragma once

#include <QAbstractListModel>

#include "AudioBook.h"
//#include "qpopka.h"

class AudioBookModel : public QAbstractListModel {

Q_OBJECT

    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(AudioBook* audioBook READ audioBook WRITE setAudioBook NOTIFY audioBookChanged)

public:

    enum AudioBookRoles {
        TextRole = Qt::UserRole +1,
        ProgressRole
    };

    AudioBookModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    AudioBook* audioBook();
    void setAudioBook(AudioBook* value);

    int index() const;
    void setIndex(int value);

signals:

    void listChanged();
    void indexChanged();
    void audioBookChanged();

private:

    int m_index = 0;
    AudioBook* m_audiobook = nullptr;

};
