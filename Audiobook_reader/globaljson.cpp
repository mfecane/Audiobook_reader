#include "globaljson.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

GlobalJSON* GlobalJSON::m_instance = nullptr;


QJsonObject GlobalJSON::getBook(QString path) {
    m_mux.lock();
    QJsonObject bookObject;
    QJsonArray bookArray;
    if(m_root.contains("books") && m_root["books"].isArray()) {
        bookArray = m_root["books"].toArray();
        for (int i = 0; i < bookArray.size(); ++i) {
            QJsonObject bookObject1 = bookArray[i].toObject();
            if(bookObject1.contains("url") &&
                    bookObject1["url"].isString() &&
                    bookObject1["url"].toString() == path) {
                bookObject = bookObject1;
            }
        }
    }
    m_mux.unlock();
    return bookObject;
}

void GlobalJSON::setBook(QJsonObject currentBookObject, QString folder) {
    m_mux.lock();
    QJsonArray bookArray;
    bool success = false;
    if(m_root.contains("books") && m_root["books"].isArray()) {
        bookArray = m_root["books"].toArray();
        for (int i = 0; i < bookArray.size(); ++i) {
            QJsonObject bookObject = bookArray[i].toObject();
            if(bookObject.contains("url") &&
                    bookObject["url"].isString() &&
                    bookObject["url"].toString() == folder) {
                bookArray.replace(i, currentBookObject);
                success = true;
            }
        }
    }
    if(!success){
        QJsonObject bookObject;
        bookArray.append(currentBookObject);
    }
    m_root["books"] = bookArray;
    m_mux.unlock();
}

bool GlobalJSON::saveJSON() {
    if(enableSave) {
        QFile saveFile(m_path);
        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return false;
        }
        QJsonDocument saveDoc(m_root);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
        return true;
    }
}

bool GlobalJSON::loadJSON() {
    QFile loadFile(m_path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    m_root = loadDoc.object();
    return true;
}


