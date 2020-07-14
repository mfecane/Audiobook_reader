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

    const AudioBookFile* item = m_audiobook->fileAt(index.row());
    switch (role) {
    case TextRole:
        return QVariant(item->fileName());
    case ProgressRole:
        return QVariant(item->progress());
    }

    return QVariant();
}

AudioBook *AudioBookModel::audioBook()
{
    return m_audiobook;
}

void AudioBookModel::setAudioBook(AudioBook *value) {
    beginResetModel();
    m_audiobook = value;
    emit audioBookChanged();
    endResetModel();
}

int AudioBookModel::index() const
{
    return m_audiobook->index();
}

void AudioBookModel::setIndex(int value)
{
    m_index = value;
    emit indexChanged();
}
