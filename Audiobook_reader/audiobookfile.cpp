#include "audiobookfile.h"

#include <QMediaMetaData>


AudioBookFile::AudioBookFile(QString sname, QObject *parent) :
    QObject(parent)
{
    m_fileName = sname;
    m_size = 0;
    m_pos = 0;
}

void AudioBookFile::metaDataChanged(bool b) {
    int size = m_player->metaData(QMediaMetaData::Duration).toInt();
    if(m_size == 0 && size != 0) {
        m_size = size;
    }
    qDebug() << "duration from metadata" << size;
}

