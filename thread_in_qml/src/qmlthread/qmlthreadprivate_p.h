#ifndef QYVLIK_QMLTHREADPRIVATE_H
#define QYVLIK_QMLTHREADPRIVATE_H

#include <QThread>
#include <QUrl>
#include <QJsonValue>
#include <QThread>
#include <QQmlEngine>

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
    void sendMessage(QJsonValue message);
    void messageReceived(QJsonValue message);

private:
    QQmlEngine* engine;
};

class QmlThreadPrivate : public QThread
{
    Q_OBJECT
public:
    explicit QmlThreadPrivate(QObject* parent=Q_NULLPTR);

    QUrl source() const;

    void setSource(const QUrl &source);

Q_SIGNALS:
    void sourceChanged(const QUrl m_source);
    void messageReceived(QJsonValue message);
    void sendMessage(QJsonValue message);

private:
    QUrl m_source;
};

} // namespace qyvlik

#endif // QYVLIK_QMLTHREADPRIVATE_H
