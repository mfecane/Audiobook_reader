#pragma once

#include <QAudioDecoder>
#include <QAudioOutput>
#include <QObject>
#include <QBuffer>
#include <QMutex>
#include <QMediaPlayer>
#include <QTimer>

#include "SoundTouchDLL.h"

#define BUFF_SIZE 13440
#define PLAY_BUFF_SIZE 44100
//6720

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    void setFile(QString fileName);
    void play();
    void pause();
    QMediaPlayer::State state() {
        return m_status;
    }

signals:
    void stateChanged(QMediaPlayer::State);

public slots:
    void bufferReady();
    void OnAudioNotify();
    void onError(QAudioDecoder::Error err);
    void handleStateChanged(QAudio::State);

private:
    QAudioFormat getPreferredFormat();
    QAudioDeviceInfo getDevice();
    void tryWritingSomeSampleData();

    QAudioDecoder m_decoder;
    QAudioDeviceInfo m_deviceinfo;
    QAudioFormat m_format;
    QAudioOutput m_audio;
    QByteArray m_fullFileBuffer;
    QByteArray m_playBuffer;
    QByteArray m_samplebuffer;
    QIODevice * m_device;
    HANDLE handle;
    int position = 0;
    int m_bytepos = 0;
    float m_pitch;

    QMutex mux;
    QMediaPlayer::State m_status;
    QTimer m_timer;

    QByteArray soundTouch(QByteArray a);
};
