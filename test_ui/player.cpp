#include "player.h"

#include <QAudioDeviceInfo>
#include <QFile>
#include <QtMath>


Player::Player(QObject *parent) : QObject(parent),
    m_samplebuffer(),
    m_smallbuffer(),
    m_audioStream(nullptr)
{
    m_state = QMediaPlayer::State::StoppedState;

    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    m_format = device.preferredFormat();

    m_audioStream = new AudioStream (m_format, this);

    m_timer.setInterval(1000);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout())); // position timer
}

void Player::play() {
    if(m_state == QMediaPlayer::State::StoppedState) {
        m_audio = new QAudioOutput(m_format, this);

        m_audio->start(m_audioStream);

        QString filename = "file.mp3";;
        QFile f(filename);
        qDebug() << "file exists " << f.exists();
        if(!f.exists()) filename = "C:/dev/file.mp3";

        m_audioStream->start(filename);
        connect(m_audioStream,SIGNAL(onFinished()),this,SLOT(stopAudio()));

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

//        delete m_audioStream;
//        m_audioStream = nullptr;

        //delete m_audio;
        //m_audio = nullptr;

        m_timer.stop();
        m_state = QMediaPlayer::State::StoppedState;
        //emit positionChanged(position());
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
    if(m_audioStream != nullptr) {
        m_audioStream->back();
    }
}

void Player::fwd(){
    if(m_audioStream != nullptr) {
        m_audioStream->fwd();
    }
}

void Player::setTempo(float t) {
    if(m_audioStream != nullptr) {
        m_audioStream->setTempo(t);
    }
}

void Player::setPosition(int pos) {
    if(m_audioStream != nullptr && m_audioStream->setPosition(pos)) {
        emit positionChanged(pos);
    }
}

int Player::position() {
    if(m_audioStream != nullptr) {
        return m_audioStream->position();
    } else {
        return 0;
    }
}

int Player::duration() {
    if(m_audioStream != nullptr) {
        return m_audioStream->duration();
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
        if (m_audio->error() != QAudio::NoError) {
            // Error handling
        }
        stop();
        break;
    }
}

void Player::timeout() {
    qDebug() << "audio state:" << m_audio->state();
    emit positionChanged(position());
}

void Player::finished() {
    stop();
}

void Player::stopAudio() {
    m_audio->stop();
}
