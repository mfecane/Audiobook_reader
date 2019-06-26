#include "AudioFileStream.h"

#include <QMutex>

AudioFileStream::AudioFileStream() :
    m_input(&m_data),
    m_output(&m_data),
    m_state(State::Stopped)
{
    setOpenMode(QIODevice::ReadOnly);

    isInited = false;
    isDecodingFinished = false;
}

// format - it is audio format to which we whant decode audio data
bool AudioFileStream::init(const QAudioFormat& format) {

    m_format = format;
    //m_format.setSampleType(QAudioFormat::Float);
    m_decoder.setAudioFormat(m_format);

    connect(&m_decoder, SIGNAL(bufferReady()), this, SLOT(bufferReady()));
    connect(&m_decoder, SIGNAL(finished()), this, SLOT(finished()));

    // Initialize buffers
    if (!m_output.open(QIODevice::ReadOnly) || !m_input.open(QIODevice::WriteOnly)) {
        return false;
    }

        handle = soundtouch_createInstance();

        uint numchannels = m_format.channelCount();
        uint sampleRate = m_format.sampleRate();

        soundtouch_setPitch(handle, 1.5);
        soundtouch_setSampleRate(handle, sampleRate);
        soundtouch_setChannels(handle, numchannels);

        qDebug() << "m_format.channelCount()" << m_format.channelCount();
        qDebug() << "m_format.sampleRate()" << m_format.sampleRate();
        qDebug() << "m_format.sampleType()" << m_format.sampleType();
        qDebug() << "m_format.codec()" << m_format.codec();

    isInited = true;

    return true;
}

// AudioOutput device (like speaker) call this function for get new audio data
qint64 AudioFileStream::readData(char* data, qint64 maxlen) {
    memset(data, 0, maxlen);

    if (m_state == State::Playing)
    {
        m_output.read(data, maxlen);

        // There is we send readed audio data via signal, for ability get audio signal for the who listen this signal.
        // Other word this emulate QAudioProbe behaviour for retrieve audio data which of sent to output device (speaker).
        if (maxlen > 0)
        {
            QByteArray buff(data, maxlen);
            emit newData(buff);
        }

        // Is finish of file
        if (atEnd())
        {
            stop();
        }
    }

    return maxlen;
}

qint64 AudioFileStream::writeData(const char* data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

// Start play audio file
void AudioFileStream::play(const QString &filePath) {
    clear();

    m_file.setFileName(filePath);

    if (!m_file.open(QIODevice::ReadOnly))
    {
        return;
    }

    m_decoder.setSourceDevice(&m_file);
    m_decoder.start();

    m_state = State::Playing;
    emit stateChanged(m_state);
}

// Stop play audio file
void AudioFileStream::stop() {
    clear();
    m_state = State::Stopped;
    emit stateChanged(m_state);
}


void AudioFileStream::clear() {
    m_decoder.stop();
    m_data.clear();
    isDecodingFinished = false;
}

// Is finish of file
bool AudioFileStream::atEnd() const {
    return m_output.size()
            && m_output.atEnd()
            && isDecodingFinished;
}



/////////////////////////////////////////////////////////////////////
// QAudioDecoder logic this methods responsible for decode audio file and put audio data to stream buffer

//void AudioFileStream::bufferReady() {

//    const QAudioBuffer &buffer = m_decoder.read();

//    //qDebug() << "buffer ready";

//    int numSamples = buffer.sampleCount() ;
//    int length = buffer.byteCount();
//    uint outNumSamples;

//    const short* data = buffer.constData<short>(); //apply filter here

//    const char* outdata[BUFF_SIZE];

//    uint numchannels = m_format.channelCount();
//    uint maxSamples = BUFF_SIZE / 2;

//    soundtouch_putSamples_i16(handle, (const short*)data, numSamples);

//    //qDebug() << "put samples";

//    do {
//        outNumSamples = soundtouch_receiveSamples_i16(handle, (short*)outdata, maxSamples);
//        uint outlength = outNumSamples;
//        //qDebug() << "get samples:" << outNumSamples;
//        m_input.write((const char*)outdata, outlength);
//    } while (outNumSamples != 0);

//    //qDebug() << "finish get samples";
//}

//Default implementation

//void AudioFileStream::bufferReady() {
//    const QAudioBuffer &buffer = m_decoder.read();
//    int length = buffer.byteCount();
//    const char* data = buffer.constData<char>();
//    m_input.write((const char*)data, length);
//}

void AudioFileStream::bufferReady() {
    const QAudioBuffer &buffer = m_decoder.read();
    int length = buffer.byteCount();
    const char* data = buffer.constData<char>();
    m_input.write((const char*)data, length);
}

// Run when decode finished decoding
void AudioFileStream::finished() {
    isDecodingFinished = true;
}

/////////////////////////////////////////////////////////////////////
