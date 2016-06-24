# QML 中的多线程

## WorkerScript

QML 中的 WorkerScript 是通过一个线程来创建一个定制过的 QQmlEngine 来实现的。

大体结构就是：

```
WorkerScript {
    id: worker
    function sendMessage(msg) {
        thread.sendMessage(msg);
    }
    signal message(messageObject)
    
    Thread {
        id: thread
        signal sendMessage(msg)
        signal message(messageObject)
        MyQmlEngine { 
            id: engine
            signal sendMessage(msg)
            signal message(messageObject)
        }
    }
    
    Component.onCompleted: {
        thread.message.connect(worker.message);
        engine.sendMessage.connect(thread.message);
        thread.sendMessage(engine.message);
    }
}
```

整体意思就是在一个线程中有一个独立的 `QQmlEngine` 实例，所有其他线程对 `QQmlEngine` 的操作都要通过 `Thread` 。

但是 `WorkerScript` 不能向线程传递一个函数或者 `QObjec*` 的实例，只能是 JSON 的数据或者 `ListModel`。

## 自定义的 QML 多线程支持

支持向线程脚本中注册 `C++` 对象。

以下为伪码，主要讲一下 `QmlThread` 的框架。

### 框架解析

算不上框架解析，只能是思路。

```
// QmlThread
QtObject {
    id: worker
    
    property url source
    property list<QmlThreadHelper> helpers
    signal message(messageObject)
    function sendMessageToThread(message) {
        // emit
        d_ptr.sendMessageToThread(message);
    }

    QmlThreadPrivate {
        id: d_ptr
        source: worker.source
    }
    
    Component.onCompleted: {
        for(var iter in helpers) {
            d_ptr.registerHelpter(helpers.name, helpers[iter]);
        }
    }
}
```

定义了一个 `QmlThread` 对象，其中包含一个 `QmlThreadPrivate` 对象和一个重要的链表属性 `helpers`。

在初始化完成后，向 `d_ptr` 注册带有名称的帮助类。

```
// QmlThreadPrivate
QtObject {
    signal sendMessageToThread(message)
    function registerHelpter(name, helper) {
        // emit
        worker.registerHelpter(name, helper)
    }

    Worker { id: worker }

    Thread { id: thread }

    Component.onCompleted: {
        worker.moveToThread(thread)
        worker.start();
    }
}
```

`QmlThreadPrivate` 对象包含一个工作实例和线程实例。在初始化完成后，将 `worker` 移动到 `thread` 线程。（worker 的 parent 设置为空）

在将 `Worker` 暴露出来的信号链接到 `QmlThreadPrivate` 的对应信号上或者方法上，以便调用。注意，只有信号是线程安全的，除非其他方法有设置线程保护。

```
// Worker

QtObject {
    id: worker
    
    signal sendMessageToThread(message)
    signal sendEvaluateToEngine(QString,QString,int)
    signal registerHelpter(name, helper);
    
    JSEngine { 
        id: engine
        function evaluate(){}
    }
    function init(){ 
        // set console etc.
        engine.setSomeGlobalObject();
    }
    function clear(){ 
        init(); 
    }
}
```

一个 `Worker` 有一个内置的，自定义的 `QJSEngine`。

### 运作流程

首先初始化。

```
// script.js
Worker.onMessage = function(message) {
    var i = message.OK + 1；
    Worker.sendMessageToThread({"OK": i});
}
```

在设定 `Thread` 的 `source` 后，就会触发 `Worker::sendEvaluateToEngine` 的信号，将读取的代码发送给 `QJSEngine` 执行。注意在执行读取到的线程脚本时，就已经对 `QJSEngine` 设置了一些必要的对象了，例如全局对象 `Worker`。

上述函数将一个函数保存到全局对象 `Worker` 的 `onMessage` 中去。

接下来等待触发 `Thread::sendMessageToWorker` 这个函数，这个函数一旦被调用，就会触发 `d_ptr->startWork`，开始触发线程中 `Worker` 的工作函数 `onMessage`。

触发栈如下：

```
QmlThread::sendMessageToWorker --> 
QmlThreadPrivate::startWork --> 
Worker::doWork --> 
Worker::work --> 
Worker.onMessage
```

在完成任务后，全局对象 `Worker` 触发 `sendMessageToThread` 信号，这个信号通过 `QmlThreadPrivate::sendMessageToThread` 信号转发给 `Thread::message`。将处理好的数据送出。

```
Worker.sendMessageToThread -->
QmlThreadPrivate::sendMessageToThread -->
QmlThread::message
```

~~改进，应该是 `JSEngine` 拥有 `Worker`。所以需要大改。不过暴露出来的接口改动却不会很大。~~

---

### 注册帮助类到 `Thread`

先看怎么使用吧。

```qml
Thread {
    id: thread
    source: "./worker.js"
    helpers: [   
        FooHelper {
            id: foo    // foo.name == FooHelper
        } 
    ]
    onMessage: {
        console.log(messageObject.OK)
        print("result:" + messageObject.OK)
    }
}
```

在 qml 文档中，在 `Thread::helpers` 列表中写入一些帮助类（继承自 `QmlThreadHelper`）。

```js
//~ worker.js
Worker.onMessage = function(message) {
    var i;

    try {
        var foo = FooHelperFactory.create();
        i = foo.helper(message.OK);
    } catch(e) {
        console.log(e);
        i = 0;
    }

    Worker.sendMessageToThread({"OK": i});
}
```

在 `JavaScript` 代码中，只能在函数内部使用 `FooHelper`，否则直接使用会提示 `undefined`。因为 `QmlThread` 的实现是先加载这个 `js` 文件，后注册帮助类。

```cpp
class QmlThreadHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString factoryName READ factoryName CONSTANT FINAL)
...
    virtual QmlThreadHelperFactory* helperFactory(QObject* parent, QJSEngine* jsEngine) = 0;
...
};
```

看看部分 `QmlThreadHelper` 的定义，定义了一个 `QmlThreadHelperFactory* QmlThreadHelper::helperFactory(QObject* parent, QJSEngine* jsEngine)` 纯虚函数，需要在继承后提供一个实现，这个函数主要是用来返回一个工厂实例，这个返回的工厂实例是属于工作线程中 `JSEngine` 的。然后将实例化的工厂类设置到 `JSEngine` 的属性中。

> 因为是多线程，所以不能直接在另一个线程的 `JSEngine` 中使用 `QmlThreadHelper` 的方法，来获取一个帮助对象实例（这个实例对象必须在线程中创建，不能在主线程创建，在主线程创建，然后在线程中使用，由于没有设置线程安全机制，容易出问题），但是可以在另一个线程使用 `QmlThreadHelper::helperFactory` 来获取一个工厂实例，由于是在线程中创建的，并且工厂实例属于 `JSEngine`，就没有线程限制。

```cpp
class QmlThreadHelperFactory : public QObject
{
    QmlThreadHelperFactory(QObject *parent, QJSEngine* engine)

    Q_INVOKABLE virtual QJSValue create() = 0;

    QJSEngine *getJsEngine() const;
...
};
```

然后重新实例化 `QmlThreadHelperFactory`，`QJSValue QmlThreadHelperFactory::create()` 是纯虚函数，需要继承后提供一个实现。

由于是将 `QmlThreadHelper::factoryName` 和 `QmlThreadHelper::helperFactory` 一起绑定，设置到 `JSEngine` 中，所以使用的时候就如下：

```
var helper = HelperFactory.create()
```

上述的 `HelperFactory` 其实是 `HelperFactory`，通过调用 `create()` 返回一个 `QJSValue` 到 `JSEngine` 的运行上下文。 

