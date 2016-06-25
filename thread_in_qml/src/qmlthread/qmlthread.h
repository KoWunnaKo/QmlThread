#ifndef QYVLIK_CHILDTHREADCONNECTOR_H
#define QYVLIK_CHILDTHREADCONNECTOR_H

#include <QObject>
#include <QJsonValue>
#include <QUrl>

class QQmlEngine;

namespace qyvlik {

class QmlRunnable : public QObject
{
    Q_OBJECT
public:
    explicit QmlRunnable(QObject* parent = nullptr) :
        QObject(parent)
    { }

Q_SIGNALS:
    void sendMessage(QJsonValue message);
    void messageReceived(QJsonValue message);
};

class QmlThreadPrivate;
class QmlThread : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit QmlThread(QObject *parent = 0);
    ~QmlThread();

   Q_INVOKABLE bool sendMessage(QJsonValue message);

public:
    QUrl source() const;
    void setSource(const QUrl &newValue);

Q_SIGNALS:
    void sourceChanged(const QUrl source);
    void messageReceived(QJsonValue message);

private:
    QmlThreadPrivate* d_ptr;
};

} // namespace qyvlik

#endif // QYVLIK_CHILDTHREADCONNECTOR_H
