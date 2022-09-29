#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QQmlContext>
#include <QDebug>
#include <QThread>

#include "backend.h"
#include "globaljson.h"
#include "audiobook.h"
#include "backend.h"
#include "lib.h"

BackEnd* BackEnd::m_instance = nullptr;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_settings("Gavitka software", "Audiobook reader"),
    m_audiobook(nullptr)
{
    for(float f = 0.5; f <= 2.0; f += 0.1) {
        m_tempoValues.append(f);
    }

    createPlayer();

    GlobalJSON::getInstance()->loadJSON();

    int tempo = m_settings.value("tempo").toInt();
    if(tempo > 0) {
        m_tempo = tempo;
    }
    setupAutosave();
}

QObject *BackEnd::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return (QObject*)getInstance();
}

BackEnd *BackEnd::getInstance()
{
    if(!m_instance) {
        m_instance = new BackEnd();
    }
    return m_instance;
}

bool BackEnd::isPlaying()
{
    return m_player->state() == QMediaPlayer::PlayingState;
}

void BackEnd::setupAutosave()
{
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    m_autoSaveTimer->setInterval(5000);
    m_autoSaveTimer->start();
}

qreal BackEnd::fileProgress()  // move to player
{
    return m_audiobook->progressCurrentFile();
}

void BackEnd::setFileProgress(qreal value)
{
    if(m_currentDuration != 0) {
        qDebug() << "setFileProgress " << value;
        int newpos = m_currentDuration * value;
        emit playerPosition(newpos);
    }
}

qreal BackEnd::bookProgress()
{
    if(m_audiobook != nullptr) {
        return m_audiobook->progress();
    } else {
        return 0;
    }
}

QString BackEnd::rootPath()
{
    return m_rootPath;
}

void BackEnd::setRootPath(QString value)
{
    m_rootPath = value;
    m_audioBookList = new AudioBookList(value);
    m_settings.setValue("rootPath", value);
    emit rootPathChanged();
}

QString BackEnd::rootPathUrl()
{
    QUrl u;
    QDir d(m_rootPath);
    u = u.fromLocalFile(d.absolutePath());
    QString s = u.toString();
    return s;
}

void BackEnd::setRootPathUrl(QString url)
{
    QUrl u = QUrl(url);
    setRootPath(u.toLocalFile());
}

void BackEnd::increaseTempo()
{
    int index = m_tempo + 1;
    if(index >= m_tempoValues.size()) return;
    m_tempo = index;
    emit playerTempo(m_tempoValues[m_tempo]);
    m_settings.setValue("tempo", m_tempo);
    emit tempoChanged();
}

void BackEnd::decreaseTempo()
{
    int index = m_tempo - 1;
    if(index < 0) return;
    m_tempo = index;
    emit playerTempo(m_tempoValues[m_tempo]);
    m_settings.setValue("tempo", m_tempo);
    emit tempoChanged();
}

void BackEnd::setAudioBook(QString path)
{
    // TODO: Unify path check
    QDir d(path);
    if(d.exists() && !path.isEmpty()) {
        if(m_audiobook != nullptr) {
            if(m_audiobook->path() == path) {
                return;
            } else {
                closeAudioBook();
            }
        }
        try {
            m_audiobook = new AudioBook(path);
            m_audioBookList->checkIndexOf(path);
            connect(m_audiobook, &AudioBook::indexChanged, m_audioBookList, &AudioBookList::dataChangedSlot);
            connect(m_audiobook, &AudioBook::indexChanged, this, &BackEnd::indexChangedSlot);
            emit audioBookChanged();

            resetPlayer();
        }
        catch(std::exception* e) {
            qDebug() << e->what();
            return;
        }
    }
}

AudioBookList *BackEnd::audioBookList()
{
    return m_audioBookList;
}

QString BackEnd::tempo()
{
    qreal value = m_tempoValues.at(m_tempo);
    return QString::number(value, 'f', 2);
}

QString BackEnd::currentTime()
{
    if(m_audiobook != nullptr) {
        QString s1 = format_time(m_audiobook->progressInt());
        QString s2 = format_time(m_audiobook->sizeTotal());
        return s1 + " / " + s2;
    }
    return QStringLiteral("..:..:.. / ..:..:.."); // TODO: add loading animation
}

qreal BackEnd::volume()
{
    return m_volume;
}

void BackEnd::setVolume(qreal value)
{
    m_volume = value;
    emit volumeChanged();
}

QString BackEnd::audioBookName()
{
    if(m_audiobook == nullptr)  return QString("");
    return m_audiobook->name();
}

void BackEnd::setEngine(QQmlApplicationEngine *engine)
{
    m_engine = engine;
}

void BackEnd::initAudioBooks()
{
    // engine should exist
    setRootPath(m_settings.value("rootPath").toString());
    autoLoad();
}

void BackEnd::autoLoad()
{
    QString savedFolder;
    int index;
    readCurrentJson(savedFolder, index);
    setAudioBook(savedFolder);
    if(m_audiobook != nullptr) {
        m_audiobook->setIndex(index);
    }
}

void BackEnd::closeAudioBook()
{
    if(isPlaying())  {
        emit playerKill();
    }

    delete m_audiobook;
    m_audiobook = nullptr;

    emit isPlayingChanged();
}

AudioBook *BackEnd::audioBook()
{
    return m_audiobook;
}

void BackEnd::play()
{
    emit playerPlay();

    m_player2 = new Player2();
    m_player2->setFile(m_audiobook->getCurrentFilePath());
    m_player2->start();
}

void BackEnd::stop()
{
    emit playerPause();
}

void BackEnd::next()
{
    m_audiobook->setNext();
    resetPlayer();
    emit playerPlay();
}

void BackEnd::prev()
{
    m_audiobook->setPrevious();
    resetPlayer();
    emit playerPlay();
}

void BackEnd::jumpfwd()
{
    emit playerJump(Player::Jump::fwd);
}

void BackEnd::jumpfwdx2()
{
    emit playerJump(Player::Jump::fwdx2);
}

void BackEnd::jumpback()
{
    emit playerJump(Player::Jump::back);
}

void BackEnd::jumpbackx2()
{
    emit playerJump(Player::Jump::backx2);
}

void BackEnd::indexChangedSlot()
{
    resetPlayer();
    emit playerPlay();
}

void BackEnd::positionChangedSlot(int pos, int dur)
{
    m_currentPosition = pos;
    m_currentDuration = dur;
    m_audiobook->setCurrentFilePos(m_currentPosition, m_currentDuration);
    emit fileProgressChanged();
}

void BackEnd::isPlayingSlot(QMediaPlayer::State state)
{
    Q_UNUSED(state)
    emit isPlayingChanged();
}

void BackEnd::autoSave()
{
    if(m_audiobook != nullptr && m_audiobook->size() > 0) {
        if(m_currentPosition > 0) {
            m_audiobook->setCurrentFilePos(m_currentPosition, m_currentDuration); // TODO: This is cis straight white man
        }
        m_audiobook->writeJson();
    }
    writeCurrentJson();
    GlobalJSON::getInstance()->saveJSON();
}

void BackEnd::onFinishedSlot()
{
    m_audiobook->setNext();
    resetPlayer();
    emit playerPlay();
    emit isPlayingChanged();
}

void BackEnd::playerStateChangedSlot(QMediaPlayer::State state)
{
    m_playerstate = state;
    emit playerStateChanged();
}

void BackEnd::readCurrentJson(QString &savedFolder, int &savedIndex)
{
    QJsonObject jsonRoot;
    jsonRoot = GlobalJSON::getInstance()->getRoot();
    if(jsonRoot.contains("currentBook") && jsonRoot["currentBook"].isString()) {
        savedFolder = jsonRoot["currentBook"].toString();
    }
    if(jsonRoot.contains("currentFile") && jsonRoot["currentFile"].isDouble()) {
        savedIndex = jsonRoot["currentFile"].toInt();
    }
}

void BackEnd::writeCurrentJson()
{
    if(m_audiobook != nullptr && m_audiobook->size() > 0){
        muxJson.lock();
        QJsonObject& jsonRoot = GlobalJSON::getInstance()->getRoot();
        jsonRoot["currentBook"] = m_audiobook->path();
        jsonRoot["currentFile"] = m_audiobook->index();
        muxJson.unlock();
    }
}

void BackEnd::createPlayer() {
//    m_thread = new QThread();
    m_player = new Player();
//    m_player->moveToThread(m_thread);

//    connect(m_thread, &QThread::finished, m_player, &QObject::deleteLater);
//    connect(this, &BackEnd::playerSetFile, m_player, &Player::setFile);
//    connect(this, &BackEnd::playerPlay, m_player, &Player::play);
//    connect(this, &BackEnd::playerPause, m_player, &Player::pause);
//    connect(this, &BackEnd::playerVolume, m_player, &Player::setVolume);
//    connect(this, &BackEnd::playerTempo, m_player, &Player::setTempo);
//    connect(this, &BackEnd::playerPosition, m_player, &Player::setPosition);
//    connect(this, &BackEnd::playerKill, m_player, &Player::die);
//    connect(this, &BackEnd::playerJump, m_player, &Player::jump);
//    connect(m_player, &Player::stateChanged, this, &BackEnd::playerStateChanged);
//    connect(m_player, &Player::positionChanged, this, &BackEnd::positionChangedSlot);
//    connect(m_player, &Player::finished, this, &BackEnd::onFinishedSlot);

//    m_thread->start();

}


void BackEnd::resetPlayer()
{
    if(m_audiobook != nullptr && m_audiobook->size() > 0) {
        QString path = m_audiobook->getCurrentFilePath();
        int pos = m_audiobook->getCurrentFilePos();
        if(QFile(path).exists()) {
            emit playerSetFile(path);
            emit playerPosition(pos);
            //m_player2->setFile(path);
        }
    }
}
