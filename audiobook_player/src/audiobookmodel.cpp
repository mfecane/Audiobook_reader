#include "audiobookmodel.h"
#include "backend.h"

AudioBookModel::AudioBookModel()
{
}

QHash<int, QByteArray> AudioBookModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[ProgressRole] = "progress";
    return roles;
}

int AudioBookModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_audiobook == nullptr)
        return 0;

    return m_audiobook->size();
}

QVariant AudioBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_audiobook)
        return QVariant();
    int i = index.row();
    const AudioBookFile& item = m_audiobook->fileAt(i);
    switch (role) {
    case TextRole:
        return QVariant(item.name);
    case ProgressRole:
        return QVariant(m_audiobook->progressOf(i));
    }

    return QVariant();
}

void AudioBookModel::setAudioBook(AudioBook *value) {
    beginResetModel();
    m_audiobook = value;
    endResetModel();
    emit indexChanged();
    QObject::connect(m_audiobook, &AudioBook::indexChanged, this, &AudioBookModel::indexChangedSlot);
}

int AudioBookModel::index() const
{
    return m_audiobook->index();
}

void AudioBookModel::indexChangedSlot(int i)
{
    QModelIndex topLeft = QAbstractItemModel::createIndex(0, 0);;
    QModelIndex bottomRight = QAbstractItemModel::createIndex(m_audiobook->size()-1, 0);;
    emit dataChanged(topLeft, bottomRight, QVector<int>({ProgressRole}));
    emit indexChanged();
}

void AudioBookModel::setIndex(int value)
{
    m_audiobook->setIndex(value);
    emit indexChanged();
}
