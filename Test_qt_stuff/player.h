#pragma once

#include <QAudioDecoder>
#include <QAudioOutput>
#include <QObject>
#include <QBuffer>
#include <QMutex>
#include <QTimer>

#include "SoundTouchDLL.h"

#define BUFF_SIZE 13440
//6720

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);

    void changePitch(float p);

signals:

public slots:
    void bufferReady();
    void OnAudioNotify();
    void onError(QAudioDecoder::Error err);
    void changeParameters();

private:
    QAudioFormat getPreferredFormat();
    QAudioDeviceInfo getDevice();
    void tryWritingSomeSampleData();
    QByteArray soundTouch(QByteArray a);

    QAudioDecoder m_decoder;
    QAudioDeviceInfo m_deviceinfo;
    QAudioFormat m_format;
    QAudioOutput m_audio;
    QByteArray m_samplebuffer;
    QIODevice * m_device;
    HANDLE handle;
    QTimer m_timer;
    int bytepos;
    float pitch;

    int channels;

};
