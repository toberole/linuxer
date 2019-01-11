### 注意一定不要在头文件中写方法的实现

### 可以通过man xxx 查看xxx系统API的文档

### Linux 网络编程

<pre>

TCP服务器端：
    1、socket()
    2、bind()
    3、listen()
    4、accept()

TCP客户端：
    1、socket()
    2、connect()
</pre>

<pre>
UDP服务端:
    1、socket
    2、bind
    3、recvfrom & sendto
    4、close

UDP客户端:
    1、socket
        1、1 可以加一个connect做优化。
    2、recvfrom & sendto
    3、close
</pre>

### API
fcntl 获取和设置文件的属性，可以利用改API设置socket为阻塞或者非阻塞

