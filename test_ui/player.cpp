#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>

Player::Player(QObject *parent = nullptr) : QIODevice(parent),
    m_outBuffer(&m_data),
    m_inBuffer(&m_data)
{
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    m_format = device.preferredFormat();

    m_decoder = new QAudioDecoder(this);
    m_decoder->setAudioFormat(m_format);

    setOpenMode(QIODevice::ReadOnly);

    m_tempo = 1.0f;
    m_jumpamount = m_format.sampleRate() * m_format.sampleSize() / 8 * 20;

    connect(m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()));
    connect(m_decoder, SIGNAL(finished()), this, SLOT(decodingFinished()));
    connect(m_decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(onError(QAudioDecoder::Error)));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkSmallBuffer()));

    m_state = QMediaPlayer::State::StoppedState;

    m_timer.setInterval(1000);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout())); // position timer

    m_audio = new QAudioOutput(m_format, this);
}

Player::~Player() {
    m_decoder->stop();
    m_data.clear();
    m_smallbuffer.clear();
    soundtouch_flush(handle);
    soundtouch_clear(handle);
}

void Player::start() {
    if(m_state == QMediaPlayer::State::StoppedState) {

        if(!m_filename.isEmpty()) {

            connect(m_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
            m_state = QMediaPlayer::State::PlayingState;
            m_timer.start();

            m_audio->start(this);
            m_decoder->setSourceFilename(m_filename);
            m_decoder->start();
            m_bytepos = 0;

            initSoundtouch();

            if (!m_outBuffer.open(QIODevice::ReadOnly) ||
                    !m_inBuffer.open(QIODevice::WriteOnly)) {
                return;
            }
        }
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
        m_smallbuffer.clear();
        m_outBuffer.close();
        m_inBuffer.close();
        m_data.clear();
        m_bytepos = 0;
        clearSoundtouch();

        m_timer.stop();
        //clear();
        //close();
        m_state = QMediaPlayer::State::StoppedState;
        emit positionChanged(position());
    }
}

void Player::pause() {
    if(m_state == QMediaPlayer::State::PlayingState) {
        m_audio->suspend();
        m_state = QMediaPlayer::State::PausedState;
    }
}

void Player::back(){
    if((m_bytepos - m_jumpamount) > 0) {
        m_bytepos -= m_jumpamount;
    }
}

void Player::fwd(){
    if(m_data.size() > (m_bytepos + m_jumpamount)) { //check if available
        m_bytepos += m_jumpamount;
    }
}

int Player::position() {
    int pos = m_bytepos
            / (m_format.sampleRate() / 1000)
            / (m_format.sampleSize() / 8)
            / m_format.channelCount();
    //qDebug() << "pos" << pos;
    return pos;
}

bool Player::setPosition(int newpos) {
    if(newpos >= 0 && newpos < m_data.size()) {
        m_bytepos = newpos * (m_format.sampleRate() / 1000)
                * (m_format.sampleSize() / 8)
                * m_format.channelCount();
        qDebug() << "set Pos" << m_bytepos
                 << " of " << m_data.size();
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
        clearSoundtouch();

        //reinitialize soundtouch();
        initSoundtouch();
    }
}

void Player::setFile(QString filename) {
    QFile f(filename);
    if(f.exists()) {
        m_filename = filename;
    }
}

qint64 Player::readData(char *data, qint64 maxlen) {
    qDebug() << "asking for more" << maxlen;

    if(maxlen > 0) {

        if (atEnd()) {
            qDebug() << "atEnd" ;
            //setOpenMode(QIODevice::NotOpen);
            stop();
            //close();
            //            emit onFinished(); //<< crash here
            //            emit aboutToClose();
            //            emit readChannelFinished();
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

void Player::checkSmallBuffer() {

    if(m_smallbuffer.size() < SMALLBUFF_SIZE) { // if smallbuffer is not full
        m_mux.lock();
        m_outBuffer.seek(m_bytepos);
        int bytesneeded = qMin(SMALLBUFF_SIZE - m_smallbuffer.size(),
                               m_data.size() - m_bytepos); // bytes we can put into smallbuff

        //Commenting this may cause performance issues
        //        if (bytesneeded < 5000) {
        //            m_mux.unlock();
        //            return;
        //        }

        //qDebug() << "in soundtuch bytes" << bytesneeded
        //         << " bytepos " << m_bytepos;

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

bool Player::atEnd() const {
    //qDebug() << "outbuffer size" << m_outBuffer.size() - m_outBuffer.pos();
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
    //numSamples = numSamples / m_tempo - 16;

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
    checkSmallBuffer();
}

void Player::decodingFinished() { // SLOT
    isDecodingFinished = true;
}

void Player::timeout() {
    qDebug() << "audio state:" << m_audio->state();
    emit positionChanged(position());
}


void Player::handleStateChanged(QAudio::State state) {
    switch (state) {
    case QAudio::IdleState:
        qDebug() << "m_audio error " << m_audio->error();
        //m_audio->stop();
        //m_audio->reset();
        break;

    case QAudio::StoppedState:
        if (m_audio->error() != QAudio::NoError) {
            // Error handling
        }
        stop();
        break;
    }
}
