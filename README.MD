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

### select
<pre>
    select本质上是通过设置或者检查存放fd标志位的数据结构来进行下一步处理。这样所带来的缺点是：

    1、单个进程可监视的fd数量被限制，即能监听端口的大小有限。一般来说这个数目和系统内存关系很大，
    具体数目可以cat/proc/sys/fs/file-max察看。32位机默认是1024个。64位机默认是2048.

    2、 对socket进行扫描时是线性扫描，即采用轮询的方法，效率较低：当套接字比较多的时候，
    每次select()都要通过遍历FD_SETSIZE个Socket来完成调度,不管哪个Socket是活跃的,都遍历一遍。
    这会浪费很多CPU时间。如果能给套接字注册某个回调函数，当他们活跃时，自动完成相关操作，
    那就避免了轮询，epoll与kqueue[unix、mac系统上的类epool]可以解决这个问题。

    3、需要维护一个用来存放大量fd的数据结构，这样会使得用户空间和
    内核空间在传递该结构时复制开销大。

</pre>

