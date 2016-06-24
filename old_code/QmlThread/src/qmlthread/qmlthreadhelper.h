#ifndef QMLTHREADHELPER
#define QMLTHREADHELPER

#include <QJSEngine>
#include <QJSValue>

class QmlThreadHelperFactory : public QObject
{
    Q_OBJECT
public:
    QmlThreadHelperFactory(QObject *parent, QJSEngine* engine):
        QObject(parent),
        jsEngine(engine)
    { }

    Q_INVOKABLE virtual QJSValue create() = 0;

    inline QJSEngine *getJsEngine() const
    { return jsEngine; }

protected:
    QJSEngine* jsEngine;
};

class QmlThreadHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString factoryName READ factoryName CONSTANT FINAL)
public:
    QmlThreadHelper(QObject *parent = Q_NULLPTR):
        QObject(parent),
        m_factoryName("ThreadHelperFactory")
    { }

    virtual QmlThreadHelperFactory* helperFactory(QObject* parent, QJSEngine* jsEngine) = 0;
    // { return Q_NULLPTR; }

    QString factoryName() const
    { return m_factoryName; }

protected:
    void setFactoryName(const QString &name)
    { m_factoryName = name; }

private:
    QString m_factoryName;
    QString m_helperInfo;
};


#endif // QMLTHREADHELPER
