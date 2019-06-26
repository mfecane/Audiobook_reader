#include <QJsonArray>
#include <QHash>
#include <QByteArray>

#include "audiobook.h"
#include "globaljson.h"
#include "backend.h"


AudioBook::AudioBook(QString path) {
    m_path = path;

    QDir d(path);
    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    d.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.mp3";
    d.setNameFilters(filters);
    if(!m_data.isEmpty()) {
        m_data.clear();
    }
    QFileInfoList list = d.entryInfoList();
    m_currentFileIndex = -1;
    for(int i = 0; i < list.size(); ++i) {
        QString sname = list.at(i).fileName();
        m_data.append(AudioBookFile(sname));
//        if(sname == m_currentFileName) {
//            m_currentFileIndex = i;
//        }
    }
    //TODO: setCurrentFile
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
    bookObject["url"] = m_path;
    GlobalJSON::getInstance()->setBook(bookObject, m_path);
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
