#pragma once

#include <QAudioDecoder>
#include <QAudioOutput>
#include <QObject>
#include <QBuffer>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QMediaPlayer>

#include "SoundTouchDLL.h"
#include "audiostream.h"

//#define BUFF_SIZE 13440
//#define SMALLBUFF_SIZE 90000
//#define NOTIFY_INTERVAL 512
//#define OUTBUFFERSIZE 30000
//6720


class Player : public QObject
{
    Q_OBJECT
public:

    explicit Player(QObject *parent = nullptr);

    void play();
    void stop();
    void pause();

    void setFile(QString fileName);
    void back();
    void fwd();

    int position();
    int duration();

    void setTempo(float t);
    void setPosition(int pos);

signals:

    void positionChanged(int pos);

private slots:

    void handleStateChanged(QAudio::State state);
    void timeout();
    void finished();

private:

    void tryWritingSomeSampleData();
    QByteArray soundTouch(QByteArray a);

    QAudioDeviceInfo m_deviceinfo;
    QAudioFormat m_format;
    QAudioDecoder* m_decoder;
    QAudioOutput* m_audio;
    QIODevice* m_device;

    HANDLE handle;

    QTimer m_timer;
    int bytepos;
    int m_position;
    int m_duration;
    QMediaPlayer::State m_state;

    int channels;
    int m_jumpamount;
    float m_tempo;
    QMutex m_mux;

    QByteArray m_samplebuffer;
    QByteArray m_smallbuffer;
    AudioStream* m_audioFileStream;
};
