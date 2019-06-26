#include <QJsonObject>
#include <QMutex>

#pragma once

class GlobalJSON
{
public:
    GlobalJSON() {
        m_path = "save.json";
    }

    static GlobalJSON* getInstance() {
        if(!m_instance) {
            m_instance = new GlobalJSON();
        }
        return m_instance;
    }

    QJsonObject getRoot() {
        return m_root;
    }

    QJsonObject getBook(QString path);
    void setBook(QJsonObject currentBookObject, QString folder);

    bool saveJSON();
    bool loadJSON();

private:

    static GlobalJSON* m_instance;

    QString m_path;
    QJsonObject m_root;
    QMutex m_mux;
};

