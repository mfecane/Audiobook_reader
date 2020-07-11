#pragma once

#include <QAudioDecoder>
#include <QObject>

#include "audiofilestream.h"

class Player : public QObject
{
    Q_OBJECT
public:
    Player();

    void init();
    void play();
    void stop();
//public slots:
//    void bufferReady();

private:
    QAudioDecoder m_decoder;
    AudioFileStream* m_audioFileStream;
};

