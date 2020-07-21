#pragma once

#include <QAudioDecoder>
#include <QBuffer>
#include <QIODevice>
#include <QMutex>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFile>
#include <QThread>

#include "SoundTouchDLL.h"

#define BUFF_SIZE 13440
#define SMALLBUFF_SIZE 60000
#define NOTIFY_INTERVAL 512
#define OUTBUFFERSIZE 30000

class Player : public QIODevice
{
Q_OBJECT

public:

    enum Jump {
        back = 0,
        backx2,
        fwd,
        fwdx2
    };

    Player(QObject *parent = nullptr);
    ~Player() override;

    void start();
    bool atEnd() const override;

    int position();
    int duration();

    bool setPosition(int newpos);
    void pause();
    void stop();

    void setFile(QString filename);

    QMediaPlayer::State state();

    void play();

signals:

    void positionChanged(int pos, int dur);
    void stateChanged(QMediaPlayer::State);
    void finished();

protected:

    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;

public slots:

    void setVolume(qreal value);
    void setTempo(float t);
    void die();
    void jump(Jump jump);

private slots:

    void bufferReady();
    void onError(QAudioDecoder::Error err);
    void decodingFinished();
    void checkSmallBuffer();
    void timeout();
    void handleStateChanged(QAudio::State state);
    void durationChangedSlot(qint64 dur);

private:

    QByteArray soundTouch(QByteArray a);
    void clear();
    void clearSoundtouch();
    void initSoundtouch();
    qint64 bytes_per_msec(int msec);

    QAudioOutput* m_audio;
    QByteArray m_data;
    QByteArray m_smallbuffer;
    QAudioDecoder* m_decoder;
    QAudioFormat m_format;
    HANDLE handle = nullptr;
    QBuffer m_outBuffer;
    QBuffer m_inBuffer;
    int m_bytepos;
    qint64 m_pos;
    QMutex m_mux;
    QTimer m_timer;
    float m_tempo;
    QMediaPlayer::State m_state;
    QString m_filename;

    qint64 m_position;
    qint64 m_duration;


    bool isDecodingFinished;
    int m_afterDecodingPos = 0;
};
