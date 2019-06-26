#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>
#include <QtMath>

Player::Player(QObject *parent) : QObject(parent),
    m_samplebuffer()
{

    m_decoder = new QAudioDecoder(this);
    m_deviceinfo = QAudioDeviceInfo::defaultOutputDevice();
    m_format = m_deviceinfo.preferredFormat();

    m_jumpamount = m_format.sampleRate() * m_format.sampleSize() / 8 * 5;

    handle = soundtouch_createInstance();

    m_decoder->setAudioFormat(m_format);

    QString filename = "file.mp3";;
    QFile f(filename);
    qDebug() << "file exists " << f.exists();
    if(!f.exists()) filename = "C:/dev/file.mp3";
    m_decoder->setSourceFilename(filename);

    m_tempo = 1.0f;

    soundtouch_setPitch(handle, 1.0f);
    soundtouch_setTempo(handle, m_tempo);
    soundtouch_setSampleRate(handle, m_format.sampleRate());
    soundtouch_setChannels(handle, m_format.channelCount());

    //    connect(m_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    //    connect(m_audio, SIGNAL(notify()), this, SLOT(OnAudioNotify()));

    connect(m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()), Qt::DirectConnection);
    connect(m_decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(onError(QAudioDecoder::Error)));

    // m_audio->setBufferSize(30580*8);

    bytepos = 0;
    m_state = QMediaPlayer::State::StoppedState;
}

Player::~Player() {
    soundtouch_flush(handle);
    soundtouch_clear(handle);
}

void Player::play() {
    if(m_state == QMediaPlayer::State::StoppedState) {
        //        m_audio->setNotifyInterval(1024);
        //        m_decoder->start();
        //        m_device = m_audio->start();
        //        m_state = QMediaPlayer::State::PlayingState;

        m_audio = new QAudioOutput(m_deviceinfo, m_format, this);
        connect(m_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        connect(m_audio, SIGNAL(notify()), this, SLOT(OnAudioNotify()));
        m_audio->setBufferSize(30580*8);

        m_audio->setNotifyInterval(1024);
        m_decoder->start();
        m_device = m_audio->start(
                    );
        m_state = QMediaPlayer::State::PlayingState;
    }

    if(m_state == QMediaPlayer::State::PausedState) {
        m_audio->resume();
        m_state = QMediaPlayer::State::PlayingState;
    }
}

void Player::stop() {
    if(m_state == QMediaPlayer::State::PlayingState ||
            m_state == QMediaPlayer::State::PausedState) {
        m_audio->stop();
        m_audio->reset();

        delete m_audio;

        soundtouch_flush(handle);
        m_state = QMediaPlayer::State::StoppedState;
        m_decoder->stop();
        m_samplebuffer.clear();
        bytepos = 0;
        emit positionChanged(position());
    }
    qDebug() << "STOP EVENT";
}

void Player::pause() {
    if(m_state == QMediaPlayer::State::PlayingState) {
        m_audio->suspend();
        m_state = QMediaPlayer::State::PausedState;
    }
}

void Player::back(){
    if((bytepos - m_jumpamount) > 0) {
        bytepos -= m_jumpamount;
    }
}

void Player::fwd(){
    if(m_samplebuffer.size() > (bytepos + m_jumpamount)) { //check if available
        bytepos += m_jumpamount;
    }
}

void Player::setTempo(float t) {
    if(m_tempo != t && t >= 1.5f && t <= 2.0f) {
        m_tempo = t;
        soundtouch_flush(handle);
        soundtouch_clear(handle);

        //reinitialize soundtouch();
        handle = soundtouch_createInstance();
        soundtouch_setPitch(handle, 1.0f);
        soundtouch_setTempo(handle, m_tempo);
        soundtouch_setSampleRate(handle, m_format.sampleRate());
        soundtouch_setChannels(handle, m_format.channelCount());
    }
}

void Player::bufferReady() {
    const QAudioBuffer &buffer = m_decoder->read();
    const char* data = buffer.data<char>();
    int length = buffer.byteCount();
    //qDebug() << "buffer ready" << length;
    QMutex mux;
    mux.lock();
    m_samplebuffer.append((const char*)data, length);
    tryWritingSomeSampleData();
    mux.unlock();
}

int Player::position() {
    m_position = bytepos
            / (m_format.sampleRate() / 1000)
            / (m_format.sampleSize() / 8)
            / m_format.channelCount();
    //qDebug() << "m_position" << m_position;
    return m_position;
}

int Player::duration() {
    return m_decoder->duration();
}

void Player::OnAudioNotify() {
    qDebug() << "notify";
    QMutex mux;
    mux.lock();
    tryWritingSomeSampleData();
    mux.unlock();
    emit positionChanged(position());
}

QByteArray Player::soundTouch(QByteArray a) {
    const short* data = (const short*)a.data();

    uint channels = m_format.channelCount();
    uint samplesize = m_format.sampleSize() / 8;

    uint numSamples = a.size() / samplesize / channels;
    //numSamples = numSamples / m_tempo - 16;

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
    int towritedevice = qMin(m_audio->bytesFree(), (m_samplebuffer.size() - bytepos));
    //qDebug() << "bytes needed" << m_audio->bytesFree()
//             << "bytes in buffer" << m_samplebuffer.size()
//             << "bytepos" << bytepos;
    if(towritedevice > 0) {
        m_device->write(soundTouch(m_samplebuffer.mid(bytepos, towritedevice)));
        bytepos += towritedevice;
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
    qDebug() << "State changed: " << state;
    switch (state) {
    case QAudio::IdleState:

        qDebug() << "m_audio error " << m_audio->error();
        //m_audio->stop();
        //m_audio->reset();
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (m_audio->error() != QAudio::NoError) {
            // Error handling
        }
        break;
    }
}
