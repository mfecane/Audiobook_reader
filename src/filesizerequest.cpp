#include <QDebug>

#include "filesizerequest.h"

FileSizeRequest::FileSizeRequest(AudioBook *audiobook)
{
    m_audiobook = audiobook;
}

void FileSizeRequest::process()
{
    if(m_audiobook->size() > 0) {
        m_index = 0;
        resetDecoder();
    }
}

void FileSizeRequest::durationChangedSlot(qint64 dur)
{
    // Handle audiobook changed
    clearDecoder();
    if(dur > 0) {
        m_audiobook->setSize(m_index, dur);
    }
    ++m_index;
    if(m_index >= m_audiobook->size()) {
        emit finished();
        return;
    }
    resetDecoder();
}

void FileSizeRequest::resetDecoder()
{
    QDir d;
    m_decoder = new QAudioDecoder(this);
    QString filename = m_audiobook->filePathAt(m_index);
    m_decoder->setSourceFilename(filename);
    connect(m_decoder, &QAudioDecoder::durationChanged, this, &FileSizeRequest::durationChangedSlot);
}

void FileSizeRequest::clearDecoder()
{
    if(m_decoder != nullptr) {
        m_decoder->stop();
        delete m_decoder;
        m_decoder = nullptr;
    }
}
