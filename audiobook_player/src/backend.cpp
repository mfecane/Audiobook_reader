#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QQmlContext>
#include <QDebug>

#include "backend.h"
#include "globaljson.h"
#include "audiobook.h"
#include "backend.h"

BackEnd* BackEnd::m_instance = nullptr;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_currentPos(0),
    m_player(this),
    m_settings("Gavitka software", "Audiobook reader"),
    m_audiobook(nullptr)
{
    for(float f = 0.5; f <= 2.0; f += 0.1) {
        m_tempoValues.append(f);
    }

    QObject::connect (&m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(positionChanged(int)),
                      this, SLOT(positionChangedSlot(int)));

    //    QObject::connect (&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
    //                      this, SLOT(mediaStatusSlot(QMediaPlayer::MediaStatus)));
    // TODO: implement


    QObject::connect (&m_player, SIGNAL(onFinished()),
                      this, SLOT(onFinishedSlot()));


    GlobalJSON::getInstance()->loadJSON();
//    autoLoad();

    int tempo = m_settings.value("tempo").toInt();
    if(tempo > 0) {
        m_tempo = tempo;
    }
//    setupAutosave();
}

QObject *BackEnd::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return (QObject*)getInstance();
}

BackEnd *BackEnd::getInstance() {
    if(!m_instance) {
        m_instance = new BackEnd();
    }
    return m_instance;
}

QUrl BackEnd::getFileUrl() {
    QDir d;
    QUrl u = QUrl::fromLocalFile(m_currentFolder + d.separator() + m_currentFileName);
    qDebug() << "getFileUrl" << u ;
    return u;
}

bool BackEnd::isPlaying() {
    return m_player.state() == QMediaPlayer::PlayingState;
}

void BackEnd::setupAutosave() {
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    m_autoSaveTimer->setInterval(5000);
    m_autoSaveTimer->start();
}

qreal BackEnd::fileProgress() {
    if(m_player.duration() != 0) {
        qreal r;
        r = (qreal)m_player.position() / m_player.duration();
        return r;
    } else {
        return 0;
    }
}

void BackEnd::setFileProgress(qreal value) {
    if(m_player.duration() != 0) {
        int newpos = m_player.duration() * value;
        m_player.setPosition(newpos);
    }
}

qreal BackEnd::bookProgress() {
    if(m_audiobook != nullptr) {
        return m_audiobook->progress();
    } else {
        return 0;
    }
}

QString BackEnd::rootPath() {
    return m_rootPath;
}

void BackEnd::setRootPath(QString value) {
    m_rootPath = value;
    m_audioBookList = new AudioBookList(value);
    m_settings.setValue("rootPath", value);
    emit rootPathChanged();
}

QString BackEnd::rootPathUrl() {
    QUrl u;
    QDir d(m_rootPath);
    u = u.fromLocalFile(d.absolutePath());
    QString s = u.toString();
    return s;
}

void BackEnd::setRootPathUrl(QString url) {
    QUrl u = QUrl(url);
    setRootPath(u.toLocalFile());
}

QList<QObject *> BackEnd::bookFileList() {
    return m_bookFileList;
}

int BackEnd::playlistIndex() {
    if(m_audiobook == nullptr) return 0;
    return m_audiobook->index();
}

void BackEnd::increaseTempo() {
    int index = m_tempo + 1;
    if(index >= m_tempoValues.size()) return;
    m_tempo = index;
    m_player.setTempo(m_tempoValues.at(m_tempo));
    m_settings.setValue("tempo", m_tempo);
    emit tempoChanged();
}

void BackEnd::decreaseTempo() {
    int index = m_tempo - 1;
    if(index < 0) return;
    m_tempo = index;
    m_player.setTempo(m_tempoValues.at(m_tempo));
    m_settings.setValue("tempo", m_tempo);
    emit tempoChanged();
}

void BackEnd::setAudioBook(QString path) {
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
            emit audioBookChanged();

            //updatePlayer();
        }
        catch(std::exception* e) {
            qDebug() << e->what();
            return;
        }
    }
}

AudioBookList *BackEnd::audioBookList() {
    return m_audioBookList;
}

QString BackEnd::tempo() {
    qreal value = m_tempoValues.at(m_tempo);
    return QString::number(value, 'f', 2);
}

QString BackEnd::currentTime() {
    if(m_audiobook != nullptr) {
        QString s1 = formatTime(m_audiobook->progressInt());
        QString s2 = formatTime(m_audiobook->totaltime());
        return s1 + " / " + s2;
    }
    return QStringLiteral("..:..:.. / ..:..:..");
}

QString BackEnd::formatTime(int msec) {
    qint64 t;
    int sec, min, hour;
    t = msec;
    sec = floor(t / 1000);
    min = floor(sec / 60);
    sec = sec % 60;
    hour = floor(min / 60);
    min = min % 60;
    return QString("%1:%2:%3").arg(int2str(hour)).arg(int2str(min)).arg(int2str(sec));
}

qreal BackEnd::volume() {
    return m_volume;
}

void BackEnd::setVolume(qreal value) {
    m_volume = value;
    emit volumeChanged();
}

void BackEnd::setEngine(QQmlApplicationEngine *engine)
{
    m_engine = engine;
}

void BackEnd::initAudioBooks()
{
    // engine should exist
    setRootPath(m_settings.value("rootPath").toString());
    setAudioBook("C:/Audiobook/Abercrombie_Joe_-_Geroi_(Golovin_K)");
}

void BackEnd::closeAudioBook() {
    if(isPlaying()) m_player.stop();

    delete m_audiobook;
    m_audiobook = nullptr;

    emit isPlayingChanged();
}

void BackEnd::updatePlayer() { // updates player with appropriate file
    if(m_audiobook != nullptr && m_audiobook->size() > 0) {
        QString path = m_audiobook->getCurrentFilePath();
        if(QFile(path).exists()) {
            if(isPlaying()) m_player.stop();
            emit isPlayingChanged();

            m_player.setFile(path);
            int pos = m_audiobook->getCurrentFile()->pos();
            m_player.setPosition(pos);
            m_player.setVolume(25);

            qDebug() << "set file " << path;
            qDebug() << "pos" << pos;
        }
    }
}

QString int2str(int i) {
    QString s;
    s = QString::number(i);
    if(s.length() != 2) {
        s = QString("0") + s.right(1);
        return s;
    } else {
        return s;
    }
}

void BackEnd::setPlaylistIndex(int value) {
    if(m_audiobook->setCurrentFileIndex(value)) {
        updatePlayer();
        emit playListIndexChanged();
        emit playlistChanged();
    }
}

void BackEnd::setPlaylistFile(QString value) {
    if(m_audiobook != nullptr && m_audiobook->setCurrentFileName(value)) {
        updatePlayer();
        emit playListIndexChanged();
        emit playlistChanged();
    }
}

AudioBook *BackEnd::audioBook() {
    return m_audiobook;
}

void BackEnd::closeBookFolder() {
    autoSave();
    m_player.stop();
    emit isPlayingChanged();
}

void BackEnd::play() {
    m_player.play();
}

void BackEnd::stop() {
    m_player.pause();
    emit isPlayingChanged();
}

void BackEnd::next() {
    m_audiobook->setNext();
    updatePlayer();
    play();
    emit playListIndexChanged();
    emit playlistChanged();
}

void BackEnd::prev() {
    m_audiobook->setPrevious();
    updatePlayer();
    play();
    emit playListIndexChanged();
    emit playlistChanged();
}

void BackEnd::jumpForeward(int sec) {
    m_player.jump(sec * 1000);
    // TODO: Try to get this to work
    //    while (m_player.jump(10000) != 0) {
    //        if(m_audiobook->setNext() == false) break;
    //    }
}

void BackEnd::jumpBack(int sec) {
    m_player.jump(-sec * 1000);
}

void BackEnd::speedUp() {
    m_player.setTempo(1.3f);
}

void BackEnd::positionChangedSlot(int pos){
    Q_UNUSED(pos)
    emit fileProgressChanged();
    emit playlistItemChanged();
}

void BackEnd::isPlayingSlot(QMediaPlayer::State state) {
    Q_UNUSED(state)
    emit isPlayingChanged();
}

void BackEnd::autoSave() {
    if(m_audiobook != nullptr && m_audiobook->size() > 0) {
        m_audiobook->setCurrentPos(m_player.position());
        m_audiobook->writeJson();
    }
    writeCurrentJson();
    GlobalJSON::getInstance()->saveJSON();
}

void BackEnd::autoLoad() {
    QString savedFolder;
    QString savedFile;
    readCurrentJson(savedFolder, savedFile);
    setAudioBook(savedFolder);
    setPlaylistFile(savedFile);
}

void BackEnd::onFinishedSlot() {
    m_audiobook->setNext();
    updatePlayer();
    play();
    emit isPlayingChanged();
    emit playListIndexChanged();
}

void BackEnd::readCurrentJson(QString &savedFolder, QString &savedFile) {
    QJsonObject jsonRoot;
    jsonRoot = GlobalJSON::getInstance()->getRoot();
    if(jsonRoot.contains("currentBook") && jsonRoot["currentBook"].isString()) {
        savedFolder = jsonRoot["currentBook"].toString();
    }
    if(jsonRoot.contains("currentFile") && jsonRoot["currentFile"].isString()) {
        savedFile = jsonRoot["currentFile"].toString();
    }
}

void BackEnd::writeCurrentJson() {
    if(m_audiobook != nullptr && m_audiobook->size() > 0){
        muxJson.lock();
        QJsonObject& jsonRoot = GlobalJSON::getInstance()->getRoot();
        jsonRoot["currentBook"] = m_audiobook->getPath();
        jsonRoot["currentFile"] = m_audiobook->getCurrentFile()->fileName();
        muxJson.unlock();
    }
}
