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

#define BUFF_SIZE 13440
//6720

class Player : public QObject
{
    Q_OBJECT
public:

    explicit Player(QObject *parent = nullptr);
    ~Player();

    void play();
    void stop();
    void pause();

    void setFile(QString fileName);
    void back();
    void fwd();

    int position();
    int duration();

    void setTempo(float t);

signals:

    void positionChanged(int pos);

public slots:

    void bufferReady();
    void OnAudioNotify();
    void onError(QAudioDecoder::Error err);

private slots:

    void handleStateChanged(QAudio::State state);

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

    QByteArray m_samplebuffer;
};
