#include <QAudioDeviceInfo>
#include <QAudioDecoder>
#include <QFile>

#include "player.h"

Player::Player(QObject *parent) : QIODevice(parent),
    handle(nullptr),
    m_outBuffer(&m_data),
    m_inBuffer(&m_data)
{
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    m_format = device.preferredFormat();

    isDecodingFinished = false;
    m_afterDecodingPos = 0;
    m_decoder = new QAudioDecoder(this);
    m_decoder->setAudioFormat(m_format);

    setOpenMode(QIODevice::ReadOnly);

    m_tempo = 1.0f;
    m_jumpamount = m_format.sampleRate() * m_format.sampleSize() / 8 * 20;

    connect(m_decoder, &QAudioDecoder::bufferReady, this, &Player::bufferReady);
    connect(m_decoder, &QAudioDecoder::finished, this, &Player::decodingFinished);
    connect(m_decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), this, &Player::onError);
    connect(&m_timer, &QTimer::timeout, this, &Player::checkSmallBuffer);

    m_state = QMediaPlayer::State::StoppedState;

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &Player::timeout);

    m_audio = new QAudioOutput(m_format, this);
}

Player::~Player() {
    m_decoder->stop();
    m_data.clear();
    m_smallbuffer.clear();
    if(handle != nullptr) {
        soundtouch_flush(handle);
        soundtouch_clear(handle);
    }
}

void Player::start() {
    if(m_state == QMediaPlayer::State::StoppedState) {

        if(!m_filename.isEmpty()) {

            connect(m_audio, &QAudioOutput::stateChanged, this, &Player::handleStateChanged);
            m_timer.start();

            m_audio->start(this);
            m_bytepos = 0;

            initSoundtouch();

            if (!m_outBuffer.open(QIODevice::ReadOnly)) {
                return;
            }
            m_state = QMediaPlayer::State::PlayingState;
            emit stateChanged(m_state);
        }
    }

    if(m_state == QMediaPlayer::State::PausedState) {
        m_audio->resume();
        m_state = QMediaPlayer::State::PlayingState;
        emit stateChanged(m_state);
    }
}

void Player::stop() {
    if(m_state == QMediaPlayer::State::PlayingState ||
            m_state == QMediaPlayer::State::PausedState) {
        m_audio->stop();
        m_audio->reset();
        m_decoder->stop();
        m_data.clear();
        m_smallbuffer.clear();
        m_outBuffer.close();
        m_inBuffer.close();
        m_bytepos = 0;
        m_afterDecodingPos = 0;
        clearSoundtouch();

        m_timer.stop();
        //clear();
        //close();
        m_state = QMediaPlayer::State::StoppedState;
        emit stateChanged(m_state);
        emit positionChanged(position(), duration());
    }
}

void Player::pause() {
    if(m_state == QMediaPlayer::State::PlayingState) {
        m_audio->suspend();
        m_state = QMediaPlayer::State::PausedState;
        emit stateChanged(m_state);
    }
}

void Player::jump(Player::Jump jump) {
    int msec;
    switch(jump) {
        case fwd:
            msec = 5000;
        break;
        case fwdx2:
            msec = 60 * 1000;
        break;
        case back:
            msec = - 5000;
        break;
        case backx2:
            msec = - 60 * 1000;
        break;
    }
    int pos = position();
    int dur = duration();

    if(msec > 0  && pos + msec < dur) {
        setPosition(pos + msec);
        return;
    }
    int a = -msec;
    if(msec < 0 && pos - a > 0) {
        setPosition(pos - a);
        return;
    }
}

void Player::setVolume(qreal value) {
    m_audio->setVolume(value);
}

int Player::position() {
    int pos = m_bytepos
            / (m_format.sampleRate() / 1000)
            / (m_format.sampleSize() / 8)
            / m_format.channelCount();
    return pos;
}

bool Player::setPosition(int newpos) {
    if(newpos >= 0 && newpos < m_data.size()) {
        m_bytepos = newpos * (m_format.sampleRate() / 1000)
                * (m_format.sampleSize() / 8)
                * m_format.channelCount();
        return true;
    } else if(!isDecodingFinished) {
        m_afterDecodingPos = newpos;
        qDebug() << "after decoding pos" << m_afterDecodingPos;
        return true;
    } else {
        return false;
    }
}

int Player::duration() {
    return m_decoder->duration();
}

void Player::setTempo(float t) {
    if(m_tempo != t && t >= 0.5f && t <= 2.0f) {
        m_tempo = t;
        if(handle != nullptr) {
            clearSoundtouch();
            initSoundtouch();
        }
    }
}

void Player::setFile(QString filename) {
    QFile f(filename);
    if(f.exists()) {
        m_filename = filename;
        stop();
        m_decoder->setSourceFilename(m_filename);
        isDecodingFinished = false;
        if (!m_inBuffer.open(QIODevice::WriteOnly)) {
            return;
        }
        m_decoder->start();
    }
}

qint64 Player::readData(char *data, qint64 maxlen) {
    if(maxlen > 0) {
        if (atEnd()) {
            stop();
            emit finished();
            return -1;
        }
        checkSmallBuffer();
        int readlen = qMin( (int)maxlen, m_smallbuffer.size());
        memset(data, 0, maxlen);
        QBuffer buff(&m_smallbuffer);
        buff.open(QIODevice::ReadOnly);
        buff.read(data, readlen);
        m_smallbuffer.remove(0, readlen);
        return maxlen;
    }
    return maxlen;
}

void Player::initSoundtouch() {
    handle = soundtouch_createInstance();
    soundtouch_setPitch(handle, 1.0f);
    soundtouch_setTempo(handle, m_tempo);
    soundtouch_setSampleRate(handle, m_format.sampleRate());
    soundtouch_setChannels(handle, m_format.channelCount());
}

void Player::clearSoundtouch() {
    soundtouch_flush(handle);
    soundtouch_clear(handle);
}

void Player::clear() {
    m_decoder->stop();
    m_data.clear();
    isDecodingFinished = false;
}

void Player::play() {
    start();
}

QMediaPlayer::State Player::state() {
    return m_state;
}

void Player::checkSmallBuffer() {

    if(m_smallbuffer.size() < SMALLBUFF_SIZE) { // if smallbuffer is not full
        m_mux.lock();
        m_outBuffer.seek(m_bytepos);
        int bytesneeded = qMin(SMALLBUFF_SIZE - m_smallbuffer.size(),
                               m_data.size() - m_bytepos);
        m_smallbuffer.append(soundTouch(m_outBuffer.read(bytesneeded)));
        m_bytepos += bytesneeded;
        m_mux.unlock();
    }
}

qint64 Player::writeData(const char *data, qint64 len) { // delete
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

void Player::die()
{
    throw std::logic_error("Not implemented");
}

bool Player::atEnd() const {
    return m_outBuffer.size()
            && m_outBuffer.atEnd()
            && isDecodingFinished;
}

QByteArray Player::soundTouch(QByteArray a) {
    uint samplesize, channels, numSamples, maxSamples, outNumSamples;
    const char* outdata[BUFF_SIZE];

    const short* data = (const short*)a.data();

    channels = m_format.channelCount();
    samplesize = m_format.sampleSize() / 8;

    numSamples = a.size() / samplesize / channels;

    outNumSamples = 0;
    maxSamples = BUFF_SIZE / 2;
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

void Player::onError(QAudioDecoder::Error err) { // SLOT
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

void Player::bufferReady() { // SLOT
    const QAudioBuffer &buffer = m_decoder->read();
    const int length = buffer.byteCount();
    const char *data = buffer.constData<char>();
    m_inBuffer.write(data, length);
}

void Player::decodingFinished() { // SLOT
    if (m_afterDecodingPos != 0) {
        setPosition(m_afterDecodingPos);
        qDebug() << "decoding finished, setting pos to" << m_afterDecodingPos;
    }
    isDecodingFinished = true;
}

void Player::timeout() {
    emit positionChanged(position(), duration());
}

void Player::handleStateChanged(QAudio::State state) {
    switch (state) {
    case QAudio::IdleState:
        break;

    case QAudio::StoppedState:
        if (m_audio->error() != QAudio::NoError) {
            qDebug() << "Audio error" << m_audio->error();
        }
        stop();
        break;
    }
}
