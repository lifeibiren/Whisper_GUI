#ifndef C_H
#define C_H

#include <QObject>

class C : public QObject
{
    Q_OBJECT
public:
    explicit C(QObject *parent = nullptr);

signals:

public slots:
};

#endif // C_H