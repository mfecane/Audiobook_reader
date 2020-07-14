#pragma once

#include <QObject>
#include <QList>
#include <QString>

class QPopka: public QObject {

    Q_OBJECT

public:

    QPopka();
    QList<QString> m_list;

};
