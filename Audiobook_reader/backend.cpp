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
    m_settings("Gavitka software", "Audiobook reader"),
    m_audiobook(nullptr),
    m_audiobookFileList(nullptr)
{


    QObject::connect (&m_myplayer, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(positionChanged(qint64)),
                      this, SLOT(positionChangedSlot(qint64)));

    QObject::connect (&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                      this, SLOT(mediaStatusSlot(QMediaPlayer::MediaStatus)));

    setRootPath(m_settings.value("rootPath").toString());

    GlobalJSON::getInstance()->loadJSON();

    //audioBook()

    //loadJson();
    //readBookArrayJson();
    //openBookFolder(m_currentFolder);

    Player player;

    setupAutosave();
}

BackEnd::~BackEnd() {
    delete m_autoSaveTimer;
}

void BackEnd::setFileName(QString value) {
    // m_audiobook->setCurrentFileIdex(value);
    //    QDir d;
    //    QFile f(m_currentFolder + d.separator() + value);
    //    if(f.exists() && !value.isEmpty()) {
    //        m_currentFileName = value;
    //        loadCurrentPos();
    //        // readRootJson(); reason: loop
    //        QMutex mux;
    //        mux.lock();
    //        m_player.setMedia(getFileUrl());
    //        m_player.setPosition(m_currentPos);
    //        m_player.setVolume(50);
    //        mux.unlock();
    //    }
}

void BackEnd::setMedia() {
    m_myplayer.setFile(m_audiobook->getCurrentFilePath());
    //m_player.setMedia(m_audiobook->getCurrentFilePathUrl());
    //m_player.setPosition(m_audiobook->getCurrentFile().pos);
    //m_player.setVolume(50);
}

void BackEnd::setCurrentBookFile(int value) {
    //    BookFile* bf = (BookFile*)m_bookFileList.at(value);
    //    qDebug() << "Playing file N:" << value ;
    //    setFileName(bf->name());
    //    m_currentBookFile = value;
    //    emit currentBookFileChanged();
    if(m_audiobook->setCurrentFileIdex(value)) {
        QMutex mux;
        mux.lock();
        setMedia();
        mux.unlock();
    }
}

void BackEnd::openAudioBook(QString folder) {
    QDir d(folder);
    if(m_audiobook != nullptr) {
        delete m_audiobook;
        delete m_audiobookFileList;
    }
    if(d.exists() && !folder.isEmpty()) {
        m_audiobook = new AudioBook(folder);
        m_audiobook->readJson();
        m_audiobookFileList = new AudioBookFileList(m_audiobook);
        emit audioBookFileListChanged();
    }
}

void BackEnd::openBookFolder(QString value) {
    setCurrentFolder(value);
    readBookArrayJson();
    //fillBookFileList();
}

void BackEnd::closeBookFolder() {
    autoSave();
    m_player.stop();
}

void BackEnd::play() {
    //m_player.play();
    m_myplayer.play();
}

void BackEnd::stop() {
    //m_player.pause();
    m_myplayer.pause();
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
    qint64 pos = m_player.position();
    qint64 max = m_player.duration();
    qint64 jump = 10000;
    if(pos + jump < max) {
        m_player.setPosition(pos + jump);
    } else {
        next();
        m_player.setPosition(pos + jump - max);
    }
}

void BackEnd::jumpBack() {
    qint64 pos = m_player.position();
    qint64 max = m_player.duration();
    qint64 jump = 10000;
    if(pos - jump >= 0) {
        m_player.setPosition(pos - jump);
    } else {
        prev(); //return value
        qint64 max = m_player.duration();
        m_player.setPosition(max - (jump - pos));
    }
}

void BackEnd::speedUp() {
    m_player.setPlaybackRate(1.3);
}

void BackEnd::isPlayingSlot(QMediaPlayer::State state) {
    Q_UNUSED(state)
    emit isPlayingChanged();
}

void BackEnd::mediaStatusSlot(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::EndOfMedia) {
        next();
    }
}

void BackEnd::autoSave() {
    //qDebug() << "saving...";
    QMutex mux;
    mux.lock();
    if(m_audiobook != nullptr) {
        m_audiobook->writeJson();
        m_audiobook->setCurrentPos(m_player.position());
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

//void BackEnd::fillBookFileList() {
//    QDir d(m_currentFolder);
//    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//    d.setSorting(QDir::Name);
//    QStringList filters;
//    filters << "*.mp3";
//    d.setNameFilters(filters);
//    if(!m_bookFileList.isEmpty()) {
//        m_bookFileList.clear();
//    }
//    QFileInfoList list = d.entryInfoList();
//    m_currentBookFile = 0;
//    for(int i = 0; i < list.size(); ++i) {
//        QString sname = list.at(i).fileName();
//        m_bookFileList.append(new BookFile(sname, 0));
//        if(sname == m_currentFileName) {
//            m_currentBookFile = i;
//        }
//    }
//    emit bookFileListChanged();
//}

bool BackEnd::loadJson() {
    QFile loadFile(m_jsonFileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    m_jsonRoot = loadDoc.object();
    return true;
}

bool BackEnd::saveJson() {
    QFile saveFile(m_jsonFileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonDocument saveDoc(m_jsonRoot);
    saveFile.write(saveDoc.toJson());
    return true;
}

void BackEnd::writeBookArrayJson() {
    muxJson.lock();
    QJsonArray bookArray;
    bool success = false;
    if(m_jsonRoot.contains("books") && m_jsonRoot["books"].isArray()) {
        bookArray = m_jsonRoot["books"].toArray();
        for (int i = 0; i < bookArray.size(); ++i) {
            QJsonObject bookObject = bookArray[i].toObject();
            if(bookObject.contains("url") &&
                    bookObject["url"].isString() &&
                    bookObject["url"].toString() == m_currentFolder) {
                writeBookJson(bookObject);
                bookArray.replace(i, bookObject);
                success = true;
            }
        }
    }
    if(!success){
        QJsonObject bookObject;
        writeBookJson(bookObject);
        bookArray.append(bookObject);
    }
    m_jsonRoot["books"] = bookArray;
    muxJson.unlock();
}

void BackEnd::writeBookJson(QJsonObject &bookObject) {
    QJsonArray fileArray;
    for(const FileTimePair &p : m_fileTimes) {
        QJsonObject fileObject;
        fileObject.insert("name", p.first);
        fileObject.insert("pos", p.second);
        fileArray.append(fileObject);
    }
    bookObject["files"] = fileArray;
    bookObject["url"] = m_currentFolder;
}

void BackEnd::readBookJson(const QJsonObject &bookObject) {
    QJsonArray fileArray;
    if(bookObject.contains("files") && bookObject["files"].isArray()) {
        fileArray = bookObject["files"].toArray();
        m_fileTimes.clear();
        for(int i = 0; i < fileArray.size(); ++i) {
            QJsonObject fileObject = fileArray[i].toObject();
            if(fileObject.contains("name") &&
                    fileObject["name"].isString() &&
                    fileObject.contains("pos") &&
                    fileObject["pos"].isDouble()) {
                QString name = fileObject["name"].toString();
                qint64 pos = fileObject["pos"].toDouble();
                m_fileTimes.append(FileTimePair(name, pos));
            }
        }
    }
}

void BackEnd::readBookArrayJson() {
    muxJson.lock();
    QJsonArray bookArray;
    if(m_jsonRoot.contains("books") && m_jsonRoot["books"].isArray()) {
        bookArray = m_jsonRoot["books"].toArray();
        for (int i = 0; i < bookArray.size(); ++i) {
            QJsonObject bookObject = bookArray[i].toObject();
            if(bookObject.contains("url") &&
                    bookObject["url"].isString() &&
                    bookObject["url"].toString() == m_currentFolder) {
                readBookJson(bookObject);
            }
        }
    }
    muxJson.unlock();
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
