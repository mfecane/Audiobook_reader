#pragma once

#include <QAudioDecoder>
#include <QBuffer>
#include <QIODevice>
#include <QMutex>
#include <QTimer>

#include "SoundTouchDLL.h"

#define BUFF_SIZE 13440
#define SMALLBUFF_SIZE 60000
#define NOTIFY_INTERVAL 512
#define OUTBUFFERSIZE 30000

class AudioStream : public QIODevice
{
Q_OBJECT

public:

    AudioStream(QAudioFormat format);
    ~AudioStream();
    void start(QString fileName);
    bool atEnd() const override;

    void setTempo(float t);
    void back();
    void fwd();
    int position();
    int duration();

    bool setPosition(int newpos);
signals:

    void onFinished();

protected:

    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;

private slots:

    void bufferReady();
    void onError(QAudioDecoder::Error err);
    void finished();
    void checkSmallBuffer();

private:

    QByteArray soundTouch(QByteArray a);

    QByteArray m_data;
    QByteArray m_smallbuffer;
    QAudioDecoder* m_decoder;
    QAudioFormat m_format;
    HANDLE handle;
    QBuffer m_sBuffer;
    QBuffer m_outBuffer;
    QBuffer m_inBuffer;
    int m_bytepos;
    QMutex m_mux;
    QTimer m_timer;
    float m_tempo;
    int m_jumpamount;
    int m_position;

    bool isDecodingFinished;
    void clear();
    void stop();
};
