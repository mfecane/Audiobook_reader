#include "filesizerequest.h"

FileSizeRequest::FileSizeRequest(int i, QString path, QObject *parent) :
    QObject(parent)
{
    m_index = i;
    m_path = path;

    m_player = new QMediaPlayer(this);

    m_player->setMedia(QUrl::fromLocalFile(path));
    connect(m_player, SIGNAL(metaDataAvailableChanged(bool)),
                    this, SLOT(metaDataChangedSlot(bool)));
    connect(this, SIGNAL(metaDataChanged(int, qint64)),
                    parent, SLOT(requestResult(int, qint64)));
}

void FileSizeRequest::metaDataChangedSlot(bool v)
{
    if(v) {
        qint64 size = m_player->metaData(QMediaMetaData::Duration).toInt();
        if(size > 0) {
            emit metaDataChanged(m_index, size);
        }
    }
}
