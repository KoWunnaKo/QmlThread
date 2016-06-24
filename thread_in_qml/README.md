# QmlThread

More powerful Thread module in QML.

直接将 `QQmlEngine` 移动到新的线程，qml 的类型注册与 `QQmlEngine` 实例化无关。然后 `qml` 的语法补全和智能提示又比 `JavaScript` 强太多了。所以决定使用 `qml` 作为线程处理的代码。

线程连接器：

```
    Button {
        text: "dirsize"
        onClicked: connector.sendMessage("C:/")
    }
    ThreadConnector {
        id: connector
        source: "./thread/thread_dir_size.qml"
        onMessageReceived: {
            console.log(message.toString())
        }
    }
```

> 接口设计参照 `WebScoket` 那样，就像主线程异步连接到了网络中一台功能强劲的电脑，将你的请求转化为运算，然后再将结果发送回主线程。

```
// thread_dir_size.qml 
// 约定文件名为小写开头，这样 QtCreator 的语法树模型就不会将主线程中的变量映射到这边。
import qyvlik.thread.runnable 0.1
import qyvlik.utility 0.1


QmlRunnable {
    id: runnable

    property var __: DirSize {
        id: dirsize
    }

    onMessageReceived: {
        console.time("dirsize")
        sendMessage(dirsize.dirSize(message));
        console.timeEnd("dirsize")
    }
}
```

```
+-------- Main Thread ----------+        +-------------- Thread -------------+
|  ThreadConnector.sendMessage  |        |                                   |
|                               |  --->  |  QmlRunnable::onMessageReceived   |
|                               |  --->  |  dirsize.dirSize(message)         |
|                               |  <---  |    QmlRunnable::sendMessage       |
|         console.log(dirsize)  |        |                                   |
+-------------------------------+        +-----------------------------------+
```
