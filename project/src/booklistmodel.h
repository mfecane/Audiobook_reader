#pragma once

#include <QAbstractListModel>

#include "booklist.h"

class BookListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(BookList *list READ list WRITE setList)

public:

    enum BookListRoles {
        TextRole = Qt::UserRole +1,
        ProgressRole
    };

    BookListModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    BookList *list() const;
    void setList(BookList *model);

signals:

    void listChanged();

private:

    BookList* m_list;

};
