// This is for QmlThread
// Don't support Qt object
// Don''t support .import

Worker.onMessage = function(message) {
    var i

    try {
        // FooHelperFactory 是在 QML 中使用 helper 注册的
        var foo = FooHelperFactory.create();
        i = foo.helper(message.OK);
    } catch(e) {
        console.log(e);
        i = 0;
    }
    //var xhr = new XMLHttpRequest;

    Worker.sendMessageToThread({"OK": i});
}
