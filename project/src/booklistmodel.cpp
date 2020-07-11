#include "booklistmodel.h"

BookListModel::BookListModel()
{

}

QHash<int, QByteArray> BookListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[ProgressRole] = "progress";
    return roles;
}

int BookListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_list)
        return 0;

    return m_list->list().size();
}

QVariant BookListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_list)
        return QVariant();

    const Book* item = m_list->list().at(index.row());
    switch (role) {
    case TextRole:
        return QVariant(item->folder());
    case ProgressRole:
        return QVariant(item->progress());
    }

    return QVariant();
}

BookList *BookListModel::list() const
{
    return m_list;
}

void BookListModel::setList(BookList *list)
{
    beginResetModel();
    m_list = list;
    endResetModel();
}
