#include "backend.h"

#include "QDebug"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

BackEnd* BackEnd::m_instance = nullptr;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_currentPos(0),
    m_settings("Gavitka software", "Audiobook reader")
{

    QObject::connect (&m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
                      this, SLOT(isPlayingSlot(QMediaPlayer::State)));

    QObject::connect (&m_player, SIGNAL(positionChanged(qint64)),
                      this, SLOT(positionChangedSlot(qint64)));

    setRootPath(m_settings.value("rootPath").toString());

    loadJson();
    readCurrentJson();
    readBookArrayJson();
    openBookFolder(m_currentFolder);

    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
    m_autoSaveTimer->setInterval(5000);
    m_autoSaveTimer->start();

}

BackEnd::~BackEnd() {
    delete m_autoSaveTimer;
}

void BackEnd::setFileName(QString value) {
    QDir d;
    QFile f(m_currentFolder + d.separator() + value);
    if(f.exists() && !value.isEmpty()) {
        m_currentFileName = value;
        loadCurrentPos();
        // readRootJson(); reason: loop
        QMutex mux;
        mux.lock();
        m_player.setMedia(getFileUrl());
        m_player.setPosition(m_currentPos);
        m_player.setVolume(50);
        mux.unlock();
    }
}

void BackEnd::setCurrentBookFile(int value) {
    BookFile* bf = (BookFile*)m_bookFileList.at(value);
    qDebug() << "Playing file N:" << value ;
    setFileName(bf->name());
    m_currentBookFile = value;
    emit currentBookFileChanged();
}

void BackEnd::openBookFolder(QString value) {
    setCurrentFolder(value);
    readBookArrayJson();
    fillBookFileList();
}

void BackEnd::closeBookFolder() {
    autoSave();
    m_player.stop();
}

void BackEnd::play() {
    m_player.play();
}

void BackEnd::stop() {
    m_player.pause();
}

void BackEnd::autoSave() {
    QMutex mux;
    mux.lock();
    m_currentPos = m_player.position();
    saveCurrentPos();
    writeBookArrayJson();
    saveJson();
    m_settings.setValue("currentFile", m_currentFileName);
    mux.unlock();
}

void BackEnd::fillBookFileList() {
    QDir d(m_currentFolder);
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    if(!m_bookFileList.isEmpty()) {
        m_bookFileList.clear();
    }
    QFileInfoList list = d.entryInfoList();
    m_currentBookFile = 0;
    for(int i = 0; i < list.size(); ++i) {
        QString sname = list.at(i).fileName();
        qDebug() << "sname" << sname;
        m_bookFileList.append(new BookFile(sname, 0));
        if(sname == m_currentFileName) {
            m_currentBookFile = i;
        }
    }
    emit bookFileListChanged();
}

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

void BackEnd::readCurrentJson() {
    if(m_jsonRoot.contains("currentPos") && m_jsonRoot["currentPos"].isDouble()) {
        setCurrentFolder(m_jsonRoot["currentBook"].toString());
    }
    if(m_jsonRoot.contains("currentPos") && m_jsonRoot["currentPos"].isDouble()) {
        setFileName(m_jsonRoot["currentFile"].toString());
    }
}

void BackEnd::writeCurrentJson() {
    muxJson.lock();
    m_jsonRoot["currentBook"] = m_currentFolder;
    m_jsonRoot["currentFile"] = m_currentFileName;
    muxJson.unlock();
}
