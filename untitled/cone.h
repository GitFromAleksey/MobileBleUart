#ifndef CONE_H
#define CONE_H

#include <QObject>

class cOne : public QObject
{
    Q_OBJECT
public:
    explicit cOne(QObject *parent = nullptr);

signals:
    void SignalFromOne(const QString &message);
public slots:
    void SlotFromOne();
private:
    QString m_message;
};

#endif // CONE_H
