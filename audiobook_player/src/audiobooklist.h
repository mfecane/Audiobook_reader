#pragma once
#include <QObject>
#include <QAbstractListModel>

#include "audiobookinfo.h"

class AudioBookList : public QObject {

    Q_OBJECT

public:

    AudioBookList(QString root, QString currnet = "");
    void setRootFolder(QString value);
    AudioBookInfo* at(int i);
    void setIndex(int value);
    int getIndex();
    int size();
    void checkIndexOf(QString path);

signals:

    void indexChanged();

private:

    QString m_root;
    QVector<AudioBookInfo*> m_list;
    int m_index = 0;

};
