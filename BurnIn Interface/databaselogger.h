#ifndef DATABASELOGGER_H
#define DATABASELOGGER_H

#include <QObject>

class databaseLogger : public QObject
{
    Q_OBJECT
public:
    explicit databaseLogger(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DATABASELOGGER_H