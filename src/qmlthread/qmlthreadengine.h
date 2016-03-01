#ifndef QMLTHREADENGINE_H
#define QMLTHREADENGINE_H


#include <QJSEngine>
#include "qmlthreadhelper.h"

// don't have console or XMLHttpRequest
class Console : public QObject
{
    Q_OBJECT
public:
    Console(QObject *parent = Q_NULLPTR);
    Q_INVOKABLE void print(const QJSValue& value);
};

class QmlThreadEngine : public QJSEngine
{
    Q_OBJECT
public:
    QmlThreadEngine(QObject *parent);

    void setContextProperty(const QString& name, const QJSValue& value);

Q_SIGNALS:
    void registerHelper(const QString& helperName, QmlThreadHelper* threadHelper);

private Q_SLOTS:
    void onRegisterHelper(const QString& helperName, QmlThreadHelper* threadHelper);

private:
    QMap<QString, QmlThreadHelperFactory*> threadHelperFactoryMap;
};


#endif // QMLTHREADENGINE_H
