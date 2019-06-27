#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>
#include <QtMath>


Player::Player(QObject *parent) : QObject(parent),
    m_samplebuffer(),
    m_smallbuffer()
{
    m_state = QMediaPlayer::State::StoppedState;

    m_timer.setInterval(1000);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

void Player::play() {
    if(m_state == QMediaPlayer::State::StoppedState) {

        QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
        QAudioFormat desire_audio_romat = device.preferredFormat();

        m_audioFileStream = new AudioStream (desire_audio_romat);
        m_audio = new QAudioOutput(desire_audio_romat);

        m_audio->start(m_audioFileStream);

        QString filename = "file.mp3";;
        QFile f(filename);
        qDebug() << "file exists " << f.exists();
        if(!f.exists()) filename = "C:/dev/file.mp3";

        m_audioFileStream->start(filename);
        connect(m_audioFileStream,SIGNAL(onFinished()),this,SLOT(finished()));

        connect(m_audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        m_state = QMediaPlayer::State::PlayingState;
        m_timer.start();
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

        delete m_audioFileStream;
        delete m_audio;
        m_audioFileStream = nullptr;
        m_audio = nullptr;
        m_timer.stop();
        m_state = QMediaPlayer::State::StoppedState;
        emit positionChanged(position());
    }
    //    qDebug() << "STOP EVENT";
}

void Player::pause() {
    if(m_state == QMediaPlayer::State::PlayingState) {
        m_audio->suspend();
        m_state = QMediaPlayer::State::PausedState;
    }
}

void Player::back(){
    if(m_audioFileStream != nullptr) {
        m_audioFileStream->back();
    }
}

void Player::fwd(){
    if(m_audioFileStream != nullptr) {
        m_audioFileStream->fwd();
    }
}

void Player::setTempo(float t) {
    if(m_audioFileStream != nullptr) {
        m_audioFileStream->setTempo(t);
    }
}

void Player::setPosition(int pos) {
    if(m_audioFileStream->setPosition(pos)) {
        emit positionChanged(pos);
    }
}

int Player::position() {
    if(m_audioFileStream != nullptr) {
        return m_audioFileStream->position();
    } else {
        return 0;
    }
}

int Player::duration() {
    if(m_audioFileStream != nullptr) {
        return m_audioFileStream->duration();
    } else {
        return 0;
    }
}

void Player::handleStateChanged(QAudio::State state) {
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

void Player::timeout() {
    emit positionChanged(position());
}

void Player::finished() {
    stop();
}
