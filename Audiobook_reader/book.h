#pragma once

#include <QObject>
#include <QVector>

#include "bookfile.h"

class Book : public QObject
{
    Q_OBJECT

public:

        static Book* createbook(QString path, QObject* parent = nullptr);

    QString folder() const;
    void setFolder(const QString &value);

    int progress() const;
    void setProgress(int value);

private:

    Book(QString path, QObject *parent = nullptr);

    QVector<BookFile*> m_list;
    QString m_folder;
    QString m_path;
    int m_progress;
    int m_index;
};
