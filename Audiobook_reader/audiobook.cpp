#include <QJsonArray>
#include <QHash>
#include <QByteArray>

#include "audiobook.h"
#include "globaljson.h"
#include "backend.h"


AudioBook::AudioBook(QString path, QObject *parent) : QObject (parent),
    m_index(0)
{
    m_path = path;

    QDir d(path);
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    QFileInfoList list = d.entryInfoList();
    for(int i = 0; i < list.size(); ++i) {
        QString sname = list.at(i).fileName();
        m_data.append(AudioBookFile(sname));
    }
    m_index = 0;
    // TODO: setCurrentFile
    // TODO: fileSizes
}

void AudioBook::readJson() {
    QJsonObject bookObject =  GlobalJSON::getInstance()->getBook(m_path);
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
                setFileTime(name, pos);
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
    for(const AudioBookFile &f : m_data) {
        QJsonObject fileObject;
        fileObject.insert("name", f.fileName);
        fileObject.insert("pos", f.pos);
        fileArray.append(fileObject);
    }
    bookObject["files"] = fileArray;
    bookObject["current"] = getCurrentFile().fileName;
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
        if ( m_data.at(i).fileName == filename) {
            setCurrentFileIdex(i);
            qDebug() << "set current file name" << filename;
            return true;
        }
    }
    return false;
}

AudioBookFile AudioBook::getCurrentFile() {
    return fileAt(m_index);
}

QString AudioBook::getCurrentFilePath() {
    QDir d;
    QString path = m_path + d.separator() + getCurrentFile().fileName;
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

void AudioBook::setFileTime(QString fileName, qint64 pos) {
    for (int i = 0; i < m_data.size(); ++i) {
        if ( m_data.at(i).fileName == fileName) {
            m_data[i].pos = pos;
        }
    }
}

BackEnd *AudioBook::backEnd(){
    return BackEnd::getInstance();
}
