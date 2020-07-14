#pragma once

#include <QAbstractListModel>

#include "audiobooklist.h"

class AudioBookListModel : public QAbstractListModel
{

    Q_OBJECT

    Q_PROPERTY(AudioBookList *list READ list WRITE setList)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)

    // PROPERTIES

public:

    AudioBookList *list() const;
    void setList(AudioBookList *model);

    int index() const;
    void setIndex(int value);

    // QAbstractListModel

public:

    enum BookListRoles {
        TextRole = Qt::UserRole + 1,
        ProgressRole
    };

    AudioBookListModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:

    void listChanged();
    void indexChanged();

public slots:

    void modelChanged();

private:

    AudioBookList* m_audiobooklist = nullptr;
    int m_index = 0;

};
