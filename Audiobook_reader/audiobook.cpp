#include <QJsonArray>
#include <QHash>
#include <QByteArray>
#include <QMediaMetaData>

#include "audiobook.h"
#include "globaljson.h"
#include "backend.h"


AudioBook::AudioBook(QString path, QObject *parent) : QObject (parent),
    m_index(0)
{
    QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat format = device.preferredFormat();

    m_path = path;

    QDir d(path);
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    QFileInfoList list = d.entryInfoList();
    for(int i = 0; i < list.size(); ++i) {
        const QFileInfo &fi = list.at(i);
        QString sname = fi.fileName();
        AudioBookFile* abf = new AudioBookFile(fi.fileName());
        m_data.append(abf);
    }
    m_index = 0;
    readJson();
}

void AudioBook::readFileSizes() {
    for(int i = 0; i < m_data.size(); ++i) {
        QString path = getFilePath(i);
        AudioBookFile *abf = m_data.at(i);
        QMediaPlayer *player = new QMediaPlayer(this);
        player->setMedia(QUrl::fromLocalFile(path));
        abf->setPlayer(player);
        connect(player, SIGNAL(metaDataAvailableChanged(bool)), abf, SLOT(metaDataChanged(bool)));
    }
}

void AudioBook::readJson() {
    QJsonObject bookObject = GlobalJSON::getInstance()->getBook(m_path);
    QJsonArray fileArray;
    if(bookObject.contains("files") && bookObject["files"].isArray()) {
        fileArray = bookObject["files"].toArray();
        for(int i = 0; i < fileArray.size(); ++i) {
            QJsonObject fileObject = fileArray[i].toObject();
            if(fileObject.contains("name") &&
                    fileObject["name"].isString() &&
                    fileObject.contains("pos") &&
                    fileObject["pos"].isDouble()) {
                QString name = fileObject["name"].toString();
                qint64 pos = fileObject["pos"].toDouble();
                qint64 size = fileObject["size"].toDouble();
                AudioBookFile *abf = findFile(name);
                if(abf != nullptr) {
                    if(pos > 0 ) abf->setPos(pos);
                    if(size > 0) abf->setSize(size);
                }
            }
        }
    }
    if(bookObject.contains("current") && bookObject["current"].isString()) {
        setCurrentFileName(bookObject["current"].toString());
    }
}

void AudioBook::writeJson() {
    QJsonObject bookObject;
    QJsonArray fileArray;
    for(const AudioBookFile* f : m_data) {
        QJsonObject fileObject;
        fileObject.insert("name", f->fileName());
        fileObject.insert("pos", f->pos());
        fileObject.insert("size", f->size());
        fileArray.append(fileObject);
    }
    bookObject["files"] = fileArray;
    bookObject["current"] = getCurrentFile()->fileName();
    bookObject["url"] = m_path;
    GlobalJSON::getInstance()->setBook(bookObject, m_path);
}

bool AudioBook::setCurrentFileIdex(int i) {
    if(m_index == i) {
        return false;
    }
    qDebug() << "set current file index" << i;
    if(i >=0 && i < m_data.size()) {
        m_index = i;
        return true;
    }
    else return false;
}

bool AudioBook::setCurrentFileName(QString filename) {
    for (int i = 0; i < m_data.size(); ++i) {
        if ( m_data.at(i)->fileName() == filename) {
            setCurrentFileIdex(i);
            qDebug() << "set current file name" << filename;
            return true;
        }
    }
    return false;
}

const AudioBookFile* AudioBook::getCurrentFile() {
    return fileAt(m_index);
}

QString AudioBook::getCurrentFilePath() {
    return getFilePath(m_index);
}

qreal AudioBook::progressOf(int i) {
    if(i == m_index) {
        const AudioBookFile* abf = fileAt(i);
        if(abf->size() > 0) {
            qreal r= (qreal)abf->pos() / abf->size();
            return r;
        }
        return 0;
    } else if(i < m_index) {
        return 1;
    } else {
        return 0;
    }
}

QString AudioBook::getFilePath(int i) {
    QDir d;
    QString path = m_path + d.separator() + m_data.at(i)->fileName();
    QFile f(path);
    if(f.exists()) {
        return path;
    } else {
        return QString("");
    }
}

bool AudioBook::setNext() {
    if(m_index >= m_data.size()) return false;
    ++m_index;
    return true;
}

bool AudioBook::setPrevious() {
    if(m_index <= 0) return false;
    --m_index;
    return true;
}

qreal AudioBook::progress() {
    qint64 totalsize = 0;
    qint64 totalpos = 0;
    for(int i = 0; i < m_data.size(); ++i) {
        totalsize += m_data.at(i)->size();
        if(i < m_index) {
            totalpos += m_data.at(i)->size();
        }
    }
    return (qreal)totalpos / totalsize;
}

void AudioBook::setFileTime(QString fileName, qint64 pos, bool overwrite) {
    for (int i = 0; i < m_data.size(); ++i) {
        if ( m_data.at(i)->fileName() == fileName) {
            if(!overwrite && m_data.at(i)->pos() != 0) continue;
            m_data[i]->setPos(pos);
        }
    }
}

AudioBookFile *AudioBook::findFile(QString fileName) {
    for (int i = 0; i < m_data.size(); ++i) {
        if ( m_data.at(i)->fileName() == fileName) {
            return m_data.at(i);
        }
    }
    return nullptr;
}

BackEnd *AudioBook::backEnd(){
    return BackEnd::getInstance();
}
