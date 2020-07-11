#include <QDir>
#include <QDebug>

#include "booklist.h"
#include "book.h"


BookList::BookList(QString root, QObject *parent):
    QObject (parent),
    m_root(root)
{
    QDir d(m_root);
    if(d.exists()) {
        d.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        d.setSorting(QDir::Name);
        QFileInfoList list = d.entryInfoList();
        for(int i = 0; i < list.size(); ++i) {
            qDebug() << "adding audiobook folder" << list.at(i).absoluteFilePath() ;
            Book* ab = Book::createbook(list.at(i).absoluteFilePath(), this);
            if(ab != nullptr) {
                qDebug() << "valid audiobook found";
                m_list.append(ab);
            }
        }
    }
}

QVector<Book *> BookList::list() const
{
    return m_list;
}

void BookList::setList(const QVector<Book *> &list)
{
    m_list = list;
}

QString BookList::root() const
{
    return m_root;
}

void BookList::setRoot(const QString &root)
{
    m_root = root;
}
