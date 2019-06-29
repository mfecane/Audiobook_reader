#include "QDebug"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

#include "backend.h"
#include "globaljson.h"
#include "audiobook.h"

BackEnd* BackEnd::m_instance = nullptr;

void BackEnd::setupAutosave() {
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    m_autoSaveTimer->setInterval(5000);
    m_autoSaveTimer->start();
}

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_currentPos(0),
    m_player(this),
    m_settings("Gavitka software", "Audiobook reader"),
    m_audiobook(nullptr),
    m_audiobookFileList(nullptr)
{

    QObject::connect (&m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(positionChanged(int)),
                      this, SLOT(positionChangedSlot(int)));

    //    QObject::connect (&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
    //                      this, SLOT(mediaStatusSlot(QMediaPlayer::MediaStatus)));
    // TODO: implement


    QObject::connect (&m_player, SIGNAL(onFinished()),
                      this, SLOT(onFinishedSlot()));

    setRootPath(m_settings.value("rootPath").toString());

    GlobalJSON::getInstance()->loadJSON();

    setupAutosave();
}

void BackEnd::setCurrentFolder(QString value) {
    QDir d = QDir(value);
    if(d.exists() &&
            !value.isEmpty() &&
            (m_audiobook == nullptr ||
            m_audiobook->getPath() != value)) {
        openAudioBook(d.absolutePath());
        emit currentFolderChanged();
    }
}

void BackEnd::setMedia() { // updates player with appropriate file
    if(m_audiobook != nullptr) {
        QString path = m_audiobook->getCurrentFilePath();
        if(QFile(path).exists()) {

            m_player.setFile(path);
            int pos = m_audiobook->getCurrentFile().pos;
            m_player.setPosition(pos);
            m_player.setVolume(25);

            qDebug() << "set file " << path;
            qDebug() << "pos" << pos;
        }
    }
}

void BackEnd::setCurrentBookFileIndex(int value) {
    if(m_audiobook->setCurrentFileIdex(value)) {
        QMutex mux;
        mux.lock();
        setMedia();
        mux.unlock();
    }
}

void BackEnd::closeAudioBook() {
    m_player.stop();

    delete m_audiobook;
    m_audiobook = nullptr;

    delete m_audiobookFileList;
    m_audiobookFileList = nullptr;
}

void BackEnd::openAudioBook(QString folder) {
    QDir d(folder);
    if(m_audiobook != nullptr) {
        closeAudioBook();
    }
    if(d.exists() && !folder.isEmpty()) {
        m_audiobook = new AudioBook(folder);
        m_audiobook->readJson();
        m_audiobookFileList = new AudioBookFileList(m_audiobook);
        setMedia();
        emit audioBookFileListChanged();
        emit currentBookFileIndexChanged();
    }
}

void BackEnd::openBookFolder(QString value) {
    setCurrentFolder(value);
    //readBookArrayJson();
    //fillBookFileList();
}

void BackEnd::closeBookFolder() {
    autoSave();
    m_player.stop();
}

void BackEnd::play() {
    m_player.play();
}

void BackEnd::stop() {
    //m_player.pause();
    m_player.pause();
}

void BackEnd::next() {
    m_audiobook->setNext();
    setMedia();
    play();
}

void BackEnd::prev() {
    m_audiobook->setPrevious();
    setMedia();
    play();
}

void BackEnd::jumpForeward() {
    m_player.jump(10000);
    // TODO: Try to get this to work
    //    while (m_player.jump(10000) != 0) {
    //        if(m_audiobook->setNext() == false) break;
    //    }
}

void BackEnd::jumpBack() {
    m_player.jump(-10000);
}

void BackEnd::speedUp() {
    m_player.setTempo(1.3f);
}

void BackEnd::positionChangedSlot(int pos){
    Q_UNUSED(pos)
    emit fileProgressChanged();
}

void BackEnd::isPlayingSlot(QMediaPlayer::State state) {
    Q_UNUSED(state)
    emit isPlayingChanged();
}

void BackEnd::autoSave() {
    //    qDebug() << "saving...";
    QMutex mux;
    mux.lock();
    if(m_audiobook != nullptr) {
        m_audiobook->setCurrentPos(m_player.position());
        m_audiobook->writeJson();
    }
    GlobalJSON::getInstance()->saveJSON();
    //m_settings.setValue("currentFile", m_currentFileName);
    mux.unlock();
}

void BackEnd::autoLoad() {
    QString savedFolder;
    QString savedFile;
    readCurrentJson(savedFolder, savedFile);
    openAudioBook(savedFolder);
}

void BackEnd::onFinishedSlot() {
    m_audiobook->setNext();
    setMedia();
    play();
}

void BackEnd::readCurrentJson(QString &savedFolder, QString &savedFile) {
    QJsonObject jsonRoot;
    jsonRoot = GlobalJSON::getInstance()->getRoot();
    if(jsonRoot.contains("currentPos") && jsonRoot["currentPos"].isDouble()) {
        savedFolder = jsonRoot["currentBook"].toString();
    }
    if(jsonRoot.contains("currentPos") && jsonRoot["currentPos"].isDouble()) {
        savedFile = jsonRoot["currentFile"].toString();
    }
}

void BackEnd::writeCurrentJson() {
    muxJson.lock();
    QJsonObject jsonRoot;
    jsonRoot = GlobalJSON::getInstance()->getRoot();
    jsonRoot["currentBook"] = m_currentFolder;
    jsonRoot["currentFile"] = m_currentFileName;
    muxJson.unlock();
}

/////////// Audio book list ////////////////////////////////////

AudioBookFileList::AudioBookFileList(AudioBook *audiobook) :
    m_audiobook(audiobook)
{ }

QHash<int, QByteArray> AudioBookFileList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    return roles;
}

int AudioBookFileList::rowCount(const QModelIndex &parent) const {
    if(m_audiobook == nullptr) return 0;
    return m_audiobook->fileCount();
}

QVariant AudioBookFileList::data(const QModelIndex &index, int role) const {
    Q_UNUSED(role);
    if(m_audiobook == nullptr) return "";

    QString s = m_audiobook->fileAt(index.row()).fileName;
    return s;
}
