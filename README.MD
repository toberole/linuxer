# 注意一定不要在头文件中写方法的实现

### Linux 网络编程

<pre>
TCP服务器端依次调用socket()、bind()、listen()之后，就会监听指定的socket地址了。
TCP客户端依次调用socket()、connect()之后就向TCP服务器发送了一个连接请求。TCP服务器监听到这个请求之后，就会调用accept()函数取接收请求，这样连接就建立好了。之后就可以开始网络I/O操作了，即类同于普通文件的读写I/O操作。

</pre>

