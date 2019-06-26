#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>
#include <QTimer>
#include <QtMath>

Player::Player(QObject *parent) : QObject(parent),
    m_audio(getDevice(), getPreferredFormat()),
    m_status(QMediaPlayer::State::StoppedState)
{
    handle = soundtouch_createInstance();

    m_decoder.setAudioFormat(m_format);

    m_audio.setBufferSize(30580*8);
    m_audio.setNotifyInterval(1024);
    m_bytepos = 0;

    connect(&m_audio, SIGNAL(notify()), this, SLOT(OnAudioNotify()));
    connect(&m_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));


    connect(&m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()));
    connect(&m_decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(onError(QAudioDecoder::Error)));

    uint numchannels = m_format.channelCount();
    uint sampleRate = m_format.sampleRate();

    soundtouch_setPitch(handle, 1.0f);
    soundtouch_setTempo(handle, 1.0f);
    soundtouch_setSampleRate(handle, sampleRate);
    soundtouch_setChannels(handle, numchannels);

    m_pitch = 1.0f;
}

void Player::setFile(QString filePath) {
    QFile f(filePath);
    if(f.exists()) {
        m_samplebuffer.clear();
        m_decoder.setSourceFilename(filePath);
    }
}

void Player::play() {

    m_decoder.start();
    //    m_audio.reset();

    m_device = m_audio.start();
    m_status = QMediaPlayer::State::PlayingState;
    emit stateChanged(m_status);
}

void Player::pause() {
    qDebug() << "m_decoder.position()" << m_decoder.position();
    soundtouch_flush(handle);

    m_status = QMediaPlayer::State::PausedState;
    emit stateChanged(m_status);
}

void Player::bufferReady() {
    //soundtouch_setPitch(handle, m_pitch);
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
    int towritedevice = qMin(m_audio.bytesFree(), (m_samplebuffer.size() - m_bytepos));
//    qDebug() << "bytes free" << m_audio.bytesFree()
//             << "m_samplebuffer.size" << m_samplebuffer.size()
//             << "m_bytepos" << m_bytepos;
    if(towritedevice > 0) {
        QByteArray a =soundTouch(m_samplebuffer.mid(m_bytepos, towritedevice));
        //m_device->write(a);
        m_bytepos += towritedevice;
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

void Player::handleStateChanged(QAudio::State state) {
    switch (state) {
    case QAudio::IdleState:
        m_audio.stop();
        m_audio.reset();
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (m_audio.error() != QAudio::NoError) {
            // Error handling
        }
        break;
    }
}

QAudioFormat Player::getPreferredFormat() {
    return m_format = getDevice().preferredFormat();
}

QAudioDeviceInfo Player::getDevice() {
    return m_deviceinfo = QAudioDeviceInfo::defaultOutputDevice();
}
