#include <QDebug>

#include "filesizerequest.h"

FileSizeRequest::FileSizeRequest(int i, QString path, QObject *parent) :
    QObject(parent)
{
    m_index = i;
    m_path = path;
}

void FileSizeRequest::process()
{
    m_player = new QMediaPlayer(this);
    m_player->setMedia(QUrl::fromLocalFile(m_path));
    connect(m_player, SIGNAL(metaDataAvailableChanged(bool)),
                    this, SLOT(metaDataChangedSlot(bool)));
}

void FileSizeRequest::metaDataChangedSlot(bool v)
{
    //qDebug() << "metaDataChangedSlot index " << m_index;
    //qDebug() << "path " << m_path;
    if(v) {
        qint64 size = m_player->metaData(QMediaMetaData::Duration).toInt();
        if(size > 0) {
            emit metaDataChanged(m_index, size);
        }
    };
    emit finished();
}
