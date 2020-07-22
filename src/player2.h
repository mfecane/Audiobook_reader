#pragma once

#include <QString>
#include <QAudioDecoder>
#include <QAudioFormat>

#include <libavutil/frame.h>
#include <libavutil/mem.h>

#include <libavcodec/avcodec.h>

struct framebuffer {
    static constexpr qint64 maxlen = 64;
    qint64 startpos;
    QByteArray m_data;
};

class Player2 : public QObject
{
Q_OBJECT

public:

    Player2();

    void setPosition(qint64 pos);
    void start(QString filename);

public slots:

    void bufferReadySlot();
    void setFile(QString filename, qint64 pos);

private:

    QAudioFormat m_format;
    QString m_filename;
    framebuffer m_data;

    qint64 m_request_pos;
    qint64 m_decode_start_pos = -1;

    void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile);
    int get_format_from_sample_fmt(const char **fmt, AVSampleFormat sample_fmt);
};
