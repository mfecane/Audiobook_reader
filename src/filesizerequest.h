#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QThread>
#include <QAudioDecoder>
#include "audiobook.h"

class FileSizeRequest : public QObject
{
    Q_OBJECT

public:

    FileSizeRequest(AudioBook* audiobook);

signals:

    void finished();

public slots:

    void process();
    void durationChangedSlot(qint64 dur);

private:

    void resetDecoder();
    void clearDecoder();

    int m_index = 0;
    QAudioDecoder* m_decoder = nullptr;
    AudioBook* m_audiobook;
};
