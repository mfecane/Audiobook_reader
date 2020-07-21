#include <QAudioBuffer>
#include <QAudioDeviceInfo>
#include <QFile>

#include "player2.h"

Player2::Player2() :
    m_decoder(this)
{
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    m_format = device.preferredFormat();
    m_decoder.setAudioFormat(m_format);
}

void Player2::setFile(QString filename, qint64 pos)
{
    if(!QFile(filename).exists()) return; // TODO: handlebetter
    m_filename = filename;
    m_request_pos = pos;
    connect(&m_decoder, &QAudioDecoder::bufferReady, this, &Player2::bufferReadySlot);
    m_decoder.setSourceFilename(filename);
    m_decoder.start();
}

void Player2::setPosition(qint64 pos)
{
//    if(pos < decoder_pos)
//    m_request_pos = pos;
}

void Player2::bufferReadySlot()
{
//    if(m_decode_start_pos == -1) {
//        m_decode_start_pos = decoder_pos;
//    }
    const QAudioBuffer &buffer = m_decoder.read();
    qint64 decoder_pos = m_decoder.position();
    //qDebug() << "m_decoder.position()" << m_decoder.position();
}
