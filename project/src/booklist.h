#pragma once

#include <QVector>

#include "book.h"

class BookList : public QObject
{
    Q_OBJECT

public:
    BookList(QString root, QObject *parent = nullptr);

    QVector<Book *> list() const;
    void setList(const QVector<Book *> &list);

    QString root() const;
    void setRoot(const QString &root);

private:
    QVector<Book*> m_list;
    QString m_root;
};
