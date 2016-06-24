#ifndef QYVLIK_CHILDTHREADCONNECTOR_H
#define QYVLIK_CHILDTHREADCONNECTOR_H

#include <QObject>
#include <QThread>
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

// moveToThread
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject* parent = nullptr);

public Q_SLOTS:
    void doWork(const QUrl& thread_qml_file);

Q_SIGNALS:
    void sendMessage(QJsonValue message);
    void messageReceived(QJsonValue message);

private:
    QQmlEngine* engine;
};

class ThreadConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit ThreadConnector(QObject *parent = 0);
    ~ThreadConnector();
public:
    QUrl source() const;
    void setSource(const QUrl &newValue);

Q_SIGNALS:
    void sourceChanged(const QUrl m_source);
    void messageReceived(QJsonValue message);
    void sendMessage(QJsonValue message);

public slots:
private:
    QUrl m_source;
    QThread thread;
};

} // namespace qyvlik

#endif // QYVLIK_CHILDTHREADCONNECTOR_H
