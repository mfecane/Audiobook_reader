#pragma once

#include <QAbstractListModel>

#include "AudioBook.h"
//#include "qpopka.h"

class AudioBookModel : public QAbstractListModel {

Q_OBJECT

    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(AudioBook* audioBook WRITE setAudioBook)

public:

    enum AudioBookRoles {
        TextRole = Qt::UserRole +1
    };

    AudioBookModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    AudioBook* audioBook();
    void setAudioBook(AudioBook* value);

    int index() const;
    void setIndex(int value);

public slots:

    void indexChangedSlot();

signals:

    void listChanged();
    void indexChanged();

private:

    AudioBook* m_audiobook = nullptr;

};
