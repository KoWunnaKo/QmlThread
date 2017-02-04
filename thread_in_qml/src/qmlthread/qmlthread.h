#ifndef QYVLIK_CHILDTHREADCONNECTOR_H
#define QYVLIK_CHILDTHREADCONNECTOR_H

#include <QObject>
#include <QJsonValue>
#include <QUrl>
#include <QQmlParserStatus>

class QThread;
class QQmlEngine;

namespace qyvlik {

// moveToThread
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject* parent = nullptr);

public Q_SLOTS:
    void doWork(const QUrl& thread_qml_file);

Q_SIGNALS:
    void sendMessage(const QJsonValue& message);

    void messageReceived(const QJsonValue& message);

private:
    QQmlEngine* mEngine;
};

class QmlRunnable : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit QmlRunnable(QObject* parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void sendMessage(const QJsonValue& message);
    void messageReceived(const QJsonValue& message);
};

class QmlThreadPrivate;
class QmlThread : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl runnableSource READ runnableSource WRITE setRunnableSource NOTIFY runnableSourceChanged FINAL)
public:
    explicit QmlThread(QObject *parent = 0);
    ~QmlThread();

   Q_INVOKABLE bool sendMessage(const QJsonValue& message);

public:
    QUrl runnableSource() const;
    void setRunnableSource(const QUrl &newValue);

Q_SIGNALS:
    void runnableSourceChanged(const QUrl runnableSource);
    void messageReceived(const QJsonValue& message);

private:
    QmlThreadPrivate* d_ptr;
};

} // namespace qyvlik

#endif // QYVLIK_CHILDTHREADCONNECTOR_H
