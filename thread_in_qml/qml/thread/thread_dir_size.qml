// thread only
// 这里你不能使用到主线程中的QQmlEngine的上下文！！！！
// 不能使用 Window 和 Item 相关类

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

