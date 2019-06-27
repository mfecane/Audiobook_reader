#include "audiostream.h"

AudioStream::AudioStream(QAudioFormat format):
    m_format(format),
    m_sBuffer(&m_smallbuffer),
    m_outBuffer(&m_data),
    m_inBuffer(&m_data)
{
    m_decoder = new QAudioDecoder(this);
    m_decoder->setAudioFormat(format);

    setOpenMode(QIODevice::ReadOnly);

    if (!m_outBuffer.open(QIODevice::ReadOnly) ||
            !m_inBuffer.open(QIODevice::WriteOnly)) {
        return;
    }

    handle = soundtouch_createInstance();
    m_tempo = 1.0f;
    m_jumpamount = m_format.sampleRate() * m_format.sampleSize() / 8 * 20;
    soundtouch_setPitch(handle, 1.0f);
    soundtouch_setTempo(handle, m_tempo);
    soundtouch_setSampleRate(handle, m_format.sampleRate());
    soundtouch_setChannels(handle, m_format.channelCount());

    connect(m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()));
    connect(m_decoder, SIGNAL(finished()), this, SLOT(finished()));
    connect(m_decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(onError(QAudioDecoder::Error)));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkSmallBuffer()));
}

qint64 AudioStream::readData(char *data, qint64 maxlen) {
    qDebug() << "asking for more";

    if(maxlen > 0) {
        checkSmallBuffer();
        int readlen = qMin( (int)maxlen, m_smallbuffer.size());
        memset(data, 0, maxlen);
        QBuffer buff(&m_smallbuffer);
        buff.open(QIODevice::ReadOnly);
        buff.read(data, readlen);
        m_smallbuffer.remove(0, readlen);

        if (atEnd()) {
            emit onFinished();
            return -1;
        }
        return maxlen;
    }
    return maxlen;
}

void AudioStream::setTempo(float t) {
    if(m_tempo != t && t >= 0.5f && t <= 2.0f) {
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

AudioStream::~AudioStream() {
    m_decoder->stop();
    m_data.clear();
    m_smallbuffer.clear();
    soundtouch_flush(handle);
    soundtouch_clear(handle);
}

void AudioStream::start(QString fileName) {

    m_decoder->setSourceFilename(fileName);
    m_decoder->start();
    m_bytepos = 0;
}


void AudioStream::back(){
    if((m_bytepos - m_jumpamount) > 0) {
        m_bytepos -= m_jumpamount;
    }
}

void AudioStream::fwd(){
    if(m_data.size() > (m_bytepos + m_jumpamount)) { //check if available
        m_bytepos += m_jumpamount;
    }
}

int AudioStream::position() {
    m_position = m_bytepos
            / (m_format.sampleRate() / 1000)
            / (m_format.sampleSize() / 8)
            / m_format.channelCount();
    //qDebug() << "m_position" << m_position;
    return m_position;
}

bool AudioStream::setPosition(int newpos) {
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

int AudioStream::duration() {
    return m_decoder->duration();
}

void AudioStream::stop() {
    clear();
}


void AudioStream::clear() {
    m_decoder->stop();
    m_data.clear();
    isDecodingFinished = false;
}

void AudioStream::checkSmallBuffer() {

    if(m_smallbuffer.size() < SMALLBUFF_SIZE) { // if smallbuffer is not full
        m_mux.lock();
        m_outBuffer.seek(m_bytepos);
        int bytesneeded = qMin(SMALLBUFF_SIZE - m_smallbuffer.size(),
                               m_data.size() - m_bytepos); // bytes we can put into smallbuff

        if (bytesneeded < 5000) {
            m_mux.unlock();
            return;
        }
        QByteArray in = m_outBuffer.read(bytesneeded);
        m_bytepos += bytesneeded;
        //m_data.mid(m_bytepos, bytesneeded);
        //qDebug() << "in soundtuch bytes" << in.length()
        //         << " bytepos " << m_bytepos;
        QByteArray ba = soundTouch(in);
        //qDebug() << "out of soundtuch bytes" << ba.length();
        m_smallbuffer.append(ba);
        // or equivalently
        // m_smallbuffer.append(soundTouch(m_samplebuffer.mid(bytepos, bytesneeded)));
        m_mux.unlock();
    }
}

qint64 AudioStream::writeData(const char *data, qint64 len) { // delete
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;
}

bool AudioStream::atEnd() const {
    return m_outBuffer.size()
            && m_outBuffer.atEnd()
            && isDecodingFinished;
}

QByteArray AudioStream::soundTouch(QByteArray a) {
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

void AudioStream::onError(QAudioDecoder::Error err) { // SLOT
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


void AudioStream::bufferReady() { // SLOT
    const QAudioBuffer &buffer = m_decoder->read();
    const int length = buffer.byteCount();
    const char *data = buffer.constData<char>();
    m_inBuffer.write(data, length);
    checkSmallBuffer();
}

void AudioStream::finished() { // SLOT
    isDecodingFinished = true;
}
