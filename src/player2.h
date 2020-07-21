#pragma once

#include <QString>
#include <QAudioDecoder>
#include <QAudioFormat>

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

public slots:

    void bufferReadySlot();
    void setFile(QString filename, qint64 pos);

private:

    QAudioDecoder m_decoder;
    QAudioFormat m_format;
    QString m_filename;
    framebuffer m_data;

    qint64 m_request_pos;
    qint64 m_decode_start_pos = -1;
};
