// thread only
// 这里你不能使用到主线程中的QQmlEngine的上下文！！！！
// 不能使用 Window 和 Item 相关类

import qyvlik.thread.runnable 0.1

QmlRunnable {
    id: runnable

    function test() {
        var url = "https://api.github.com/zen"
        var xhr = new XMLHttpRequest;
        xhr.onreadystatechange = function() {
            if(xhr.readyState === xhr.DONE) {
                sendMessage(xhr.responseText);
            }
        }
        xhr.open("GET", url);
        xhr.send();
    }

    onMessageReceived: {
        if(message === "run") {
            test();
        }
    }
}

