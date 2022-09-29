#pragma once

#include <QString>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QBuffer>

extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>
    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
}

class Player2 : public QIODevice
{
Q_OBJECT

public:

    Player2();

    void setPosition(qint64 pos);
    void start();

public slots:

    void setFile(QString filename);
    void handleStateChanged(QAudio::State);
    void notifySlot();

signals:

    void finished();

protected:

    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;

private:

    void printAudioFrameInfo(const AVCodecContext *codecContext, const AVFrame *frame, AVFrame *output, SwrContext *swr);
    void decode(AVCodecContext* codecContext, AVPacket* pkt, AVFrame* frame, AVFrame *output, AVStream* audioStream, SwrContext *swr);

    QAudioFormat m_format;
    QString m_filename;

    QByteArray* m_dataframe;

    QAudioOutput* m_audio;

    QBuffer* m_readBuffer = nullptr;
};
