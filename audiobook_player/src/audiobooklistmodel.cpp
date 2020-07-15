#pragma once

#include <QDebug>

#include "audiobooklistmodel.h"
#include "audiobookinfo.h"

AudioBookListModel::AudioBookListModel()
{
}

void AudioBookListModel::indexChangedSlot()
{
    emit listChanged();
    emit indexChanged();
}

QHash<int, QByteArray> AudioBookListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[ProgressRole] = "progress";
    return roles;
}

int AudioBookListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_audiobooklist)
        return 0;

    return m_audiobooklist->size();
}

QVariant AudioBookListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (m_audiobooklist == nullptr))
        return QVariant();

    AudioBookInfo* item = m_audiobooklist->at(index.row());
    switch (role) {
    case TextRole:
        return QVariant(item->name());
    case ProgressRole:
        return QVariant(item->progress());
    }

    return QVariant();
}

AudioBookList *AudioBookListModel::list() const
{
    return m_audiobooklist;
}

void AudioBookListModel::setList(AudioBookList *list)
{
    if(list == nullptr) return;
    beginResetModel();
    m_audiobooklist = list;
    QObject::connect(m_audiobooklist, &AudioBookList::indexChanged, this, &AudioBookListModel::indexChangedSlot);
    QObject::connect(m_audiobooklist, &AudioBookList::dataChanged, this, &AudioBookListModel::dataChangedSlot);
    m_index = m_audiobooklist->getIndex();
    emit indexChanged();
    endResetModel();
}

int AudioBookListModel::index() const
{
    return m_audiobooklist->getIndex();
}

void AudioBookListModel::dataChangedSlot()
{
    QModelIndex topLeft = QAbstractItemModel::createIndex(0, 0);;
    QModelIndex bottomRight = QAbstractItemModel::createIndex(m_audiobooklist->size()-1, 0);;
    emit dataChanged(topLeft, bottomRight, QVector<int>({ProgressRole}));
}

void AudioBookListModel::setIndex(int value)
{
    if(m_audiobooklist->size() > 0) {
        m_audiobooklist->setIndex(value);
        emit indexChanged();
    }
}
