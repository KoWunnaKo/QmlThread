# QmlThread

More powerful Thread module in QML.

> [old_code](old_code/QmlThread/README.md)

## 新的想法

> [thread_in_qml](thread_in_qml/README.md)

之前的设计思路是将 `QJSEngine` 移动到新线程，并为线程中的 `QJSEngine` 设计一套注册机制。现在看来拙劣至极。

直接将 `QQmlEngine` 移动到新的线程，qml 的类型注册与 `QQmlEngine` 实例化无关。然后 `qml` 的语法补全和智能提示又比 `JavaScript` 强太多了。所以决定使用 `qml` 作为线程处理的代码。

线程连接器：

```
    Thread {
        id: thread
        source: "./thread/thread_dir_size.qml"
        onMessageReceived: {
            console.log(message.toString())
        }
    }
```

> 接口设计可能参照 `WebScoket` 那样，就像主线程异步连接到了网络中一台功能强劲的电脑，将你的请求转化为运算，然后再将结果发送回主线程。

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

[讨论](http://qtdream.com/topic/533/qml-%E4%B8%8B%E7%9A%84%E5%A4%9A%E7%BA%BF%E7%A8%8B)
