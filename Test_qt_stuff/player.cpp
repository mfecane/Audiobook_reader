#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>
#include <QtMath>

Player::Player(QObject *parent) : QObject(parent),
    m_audio(getDevice(), getPreferredFormat()),
    m_samplebuffer()
{
    //soundtouch_clear(handle);
    handle = soundtouch_createInstance();

    m_decoder.setAudioFormat(m_format);
    QString filename = "file.mp3";
    QFile f(filename);
    qDebug() << "file exists " << f.exists();
    if(!f.exists()) filename = "C:/dev/file.mp3";
    m_decoder.setSourceFilename(filename);

    uint numchannels = m_format.channelCount();
    channels = numchannels;
    uint sampleRate = m_format.sampleRate();

    pitch = 0.8f;
    soundtouch_setPitch(handle, pitch);
    soundtouch_setTempo(handle, 1.1f);
    soundtouch_setSampleRate(handle, sampleRate);
    soundtouch_setChannels(handle, numchannels);

    connect(&m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()));
    connect(&m_decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(onError(QAudioDecoder::Error)));

    m_decoder.start();

    connect(&m_audio, SIGNAL(notify()), this, SLOT(OnAudioNotify()));

    m_audio.setBufferSize(30580*8);
    m_audio.setNotifyInterval(1024);

    m_device = m_audio.start();
    bytepos = 0;

    QTimer::singleShot(25 * 1000, this, SLOT(changeParameters()));
    m_timer.start();
}

void Player::changePitch(float p) {
    pitch = p;
    soundtouch_flush(handle);
    soundtouch_clear(handle);
    handle = soundtouch_createInstance();
    soundtouch_setPitch(handle, pitch);
    soundtouch_setTempo(handle, 1.1f);
    uint numchannels = m_format.channelCount();
    uint sampleRate = m_format.sampleRate();
    soundtouch_setSampleRate(handle, sampleRate);
    soundtouch_setChannels(handle, numchannels);
}

QAudioFormat Player::getPreferredFormat() {
    m_format = getDevice().preferredFormat();
    //m_format.setSampleRate(22050);
    return m_format;
}

QAudioDeviceInfo Player::getDevice() {
    return m_deviceinfo = QAudioDeviceInfo::defaultOutputDevice();
}

void Player::bufferReady() {
    soundtouch_setPitch(handle, pitch);
    const QAudioBuffer &buffer = m_decoder.read();
    const char* data = buffer.data<char>();
    int length = buffer.byteCount();
    QMutex mux;
    mux.lock();
    m_samplebuffer.append((const char*)data, length);
    tryWritingSomeSampleData();
    mux.unlock();
}

void Player::OnAudioNotify() {
    qDebug() << "pitch" << pitch;
    QMutex mux;
    mux.lock();
    tryWritingSomeSampleData();
    mux.unlock();
}

QByteArray Player::soundTouch(QByteArray a) {
    const short* data = (const short*)a.data();

    uint channels = m_format.channelCount();
    uint samplesize = m_format.sampleSize() / 8;

    uint numSamples = a.size() / samplesize / channels;
    uint outNumSamples = 0;
    uint maxSamples = BUFF_SIZE / 2;
    const char* outdata[BUFF_SIZE];
    QByteArray out;

    soundtouch_putSamples_i16(handle, data, numSamples);

    while ((outNumSamples = soundtouch_receiveSamples_i16(handle, (short*)outdata, maxSamples)) != 0) {
        QMutex mux;
        mux.lock();
        out.append(QByteArray((const char*)outdata,
                              outNumSamples * samplesize * channels));
        mux.unlock();
    }
    return out;
}

void Player::tryWritingSomeSampleData() {
    int towritedevice = qMin(m_audio.bytesFree(), (m_samplebuffer.size() - bytepos));
    if(towritedevice > 0) {
        m_device->write(soundTouch(m_samplebuffer.mid(bytepos, towritedevice)));
        bytepos += towritedevice;
        //m_samplebuffer.remove(0, towritedevice);
        qDebug() << "bytepos" << bytepos;
        //pop front what is written
    }
}

void Player::onError(QAudioDecoder::Error err) {
    switch(err){
    case QAudioDecoder::ResourceError:
        qDebug() << "error: QAudioDecoder::ResourceError"; break;
    case QAudioDecoder::FormatError:
        qDebug() << "error: QAudioDecoder::FormatError"; break;
    case QAudioDecoder::AccessDeniedError:
        qDebug() << "error: QAudioDecoder::AccessDeniedError"; break;
    case QAudioDecoder::ServiceMissingError:
        qDebug() << "error: QAudioDecoder::ServiceMissingError"; break;
    }
}

void Player::changeParameters() {
    changePitch(1.2f);
    qDebug() << "Single shot fired";
    bytepos -= 200000;
}
