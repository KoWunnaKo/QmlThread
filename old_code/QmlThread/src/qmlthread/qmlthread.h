#ifndef QMLTHREAD_H
#define QMLTHREAD_H

#include <QObject>
#include <QUrl>
#include <QJsonValue>
#include <QJSValue>
#include <QJSEngine>
#include <QQmlListProperty>

#include "qmlthreadhelper.h"

class QmlThreadPrivate;

class QmlThread : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlListProperty<QmlThreadHelper> helpers READ helpers)

public:
    explicit QmlThread(QObject *parent = 0);

    ~QmlThread();

    QUrl source() const;

    void setSource(const QUrl& value);

    QQmlListProperty<QmlThreadHelper> helpers();

public Q_SLOTS:
    void sendMessageToWorker(QJsonValue message);

Q_SIGNALS:
    void sourceChanged();
    void message(QJsonValue messageObject);

protected:
        /*Q_INVOKABLE*/
    void registerThreadHelper(const QString& name, QmlThreadHelper* threadHelper);

    static void helpers_append(QQmlListProperty<QmlThreadHelper> *list, QmlThreadHelper *value);

    static QmlThreadHelper* helpers_at(QQmlListProperty<QmlThreadHelper> *list, int index);

    static void helpers_clear(QQmlListProperty<QmlThreadHelper> *list);

    static int helpers_count(QQmlListProperty<QmlThreadHelper> *list);

private:
    QmlThreadPrivate* d_ptr;
    QList<QmlThreadHelper*> m_threadHelpers;
};

#endif // QMLTHREAD_H
