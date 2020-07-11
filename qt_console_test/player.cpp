#include "player.h"
#include <QAudioOutput>
#include <QAudioDecoder>

#include "audiofilestream.h"

//extern "C" {
//#include "SoundTouchDLL.h"
//}

Player::Player() {
//    const float* samples;
//    uint numChannels = 2;

//    QAudioDecoder m_decoder;



//    HANDLE handle = soundtouch_createInstance();
//    soundtouch_putSamples(handle, samples, numChannels );

//    soundtouch_destroyInstance(handle);
}

void Player::init() {
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat desire_audio_romat = device.preferredFormat();

    qDebug() << "desire_audio_romat" << desire_audio_romat.sampleType();
    //desire_audio_romat.setSampleType(QAudioFormat::Float);

    m_audioFileStream = new AudioFileStream;
    if (!m_audioFileStream->init(desire_audio_romat))
    {
        return;
    }

    QAudioOutput* m_audioOutput = new QAudioOutput(desire_audio_romat);
    m_audioOutput->start(m_audioFileStream);
}

void Player::play() {
    m_audioFileStream->play("C:/file.mp3");
}

void Player::stop() {
    m_audioFileStream->stop();
}
