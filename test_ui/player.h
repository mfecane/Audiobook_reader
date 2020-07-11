#pragma once

#include <QAudioDecoder>
#include <QBuffer>
#include <QIODevice>
#include <QMutex>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFile>

#include "SoundTouchDLL.h"

#define BUFF_SIZE 13440
#define SMALLBUFF_SIZE 60000
#define NOTIFY_INTERVAL 512
#define OUTBUFFERSIZE 30000

class Player : public QIODevice
{
Q_OBJECT

public:

    Player(QObject *parent);
    ~Player();

    void start();
    bool atEnd() const override;

    void setTempo(float t);
    void back();
    void fwd();
    int position();
    int duration();

    bool setPosition(int newpos);
    void pause();
    void stop();

    void setFile(QString filename);

signals:

    void onFinished();
    void positionChanged(int pos);

protected:

    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;

private slots:

    void bufferReady();
    void onError(QAudioDecoder::Error err);
    void decodingFinished();
    void checkSmallBuffer();
    void timeout();
    void handleStateChanged(QAudio::State state);

private:

    QByteArray soundTouch(QByteArray a);
    void clear();
    void clearSoundtouch();
    void initSoundtouch();

    QAudioOutput* m_audio;
    QByteArray m_data;
    QByteArray m_smallbuffer;
    QAudioDecoder* m_decoder;
    QAudioFormat m_format;
    HANDLE handle;
    QBuffer m_outBuffer;
    QBuffer m_inBuffer;
    int m_bytepos;
    QMutex m_mux;
    QTimer m_timer;
    float m_tempo;
    int m_jumpamount;
    QMediaPlayer::State m_state;
    QString m_filename;

    bool isDecodingFinished;
};
