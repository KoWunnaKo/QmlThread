# 自定义的 QML 多线程支持

More powerful Thread module in QML. Like WorkerScript but can use c++ native method in WorkerScript.


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

[更多信息](doc/readme.md)

## TODO

1. 优化设计。

2. 错误处理。

3. 本人懒癌晚期，有空再说。