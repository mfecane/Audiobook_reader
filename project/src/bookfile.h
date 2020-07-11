#pragma once

#include <QObject>

class BookFile : public QObject
{
    Q_OBJECT

public:

    BookFile(QString fileName, QObject* parent = nullptr);

    QString fileName() const;
    void setFileName(const QString &fileName);

    int size() const;
    void setSize(int size);

    int pos() const;
    void setPos(int pos);

private:

    QString m_fileName;
    int m_size;
    int m_pos;
};
