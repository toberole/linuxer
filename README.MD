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

### epoll
<pre>
epoll API

int epoll_create(int size);
创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大。
这个参数不同于select()中的第一个参数，给出最大监听的fd+1的值。
需要注意的是，当创建好epoll句柄后，它就是会占用一个fd值，在linux下如果查看/proc/进程id/fd/，
能够看到这个fd，所以在使用完epoll后，必须调用close()关闭，否则可能导致fd被耗尽。

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll的事件注册函数，它不同与select()是在监听事件时告诉内核要监听什么类型的事件，
而是在这里先注册要监听的事件类型。第一个参数是epoll_create()的返回值，
第二个参数表示动作，用三个宏来表示：
EPOLL_CTL_ADD：注册新的fd到epfd中；
EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
EPOLL_CTL_DEL：从epfd中删除一个fd；
第三个参数是需要监听的fd
第四个参数是告诉内核需要监听什么事
events可以是以下几个宏的集合：
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET：将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里


3. int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
等待事件的产生，类似于select()调用。参数events用来从内核得到事件的集合，maxevents告之内核这个events
有多大，这个 maxevents的值不能大于创建epoll_create()时的size，
参数timeout是超时时间（毫秒，0会立即返回，-1将不确定，也有说法说是永久阻塞）。
该函数返回需要处理的事件数目，如返回0表示已超时


ET、LT两种工作模式： 默认是LT水平触发
ET模式仅当状态发生变化的时候才获得通知,这里所谓的状态的变化并不包括缓冲区中还有未处理的数据,
也就是说,如果要采用ET模式,需要一直read/write直到出错为止,很多人反映为什么采用ET模式
只接收了一部分数据就再也得不到通知了,大多因为这样;而LT模式是只要有数据没有处理就会一直通知下去的.
比如：客户端一次发送了1000字节数据，在LT模式下面一次只读了500字节，再次epoll_wait的时候，会再次返回有数据到来。
但是如果在LT模式下面，就不会返回，除非是有数据到来才会返回。


epoll变成基本框架
for( ; ; )
{
     nfds = epoll_wait(epfd,events,20,500);
     for(i=0;i<nfds;++i)
     {
         if(events[i].data.fd==listenfd) //有新的连接
         {
             connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen); //accept这个连接
             ev.data.fd=connfd;
             ev.events=EPOLLIN|EPOLLET;
             epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev); //将新的fd添加到epoll的监听队列中
         }
         else if( events[i].events&EPOLLIN ) //接收到数据，读socket
         {
             n = read(sockfd, line, MAXLINE)) < 0    //读
             ev.data.ptr = md;     //md为自定义类型，添加数据
             ev.events=EPOLLOUT|EPOLLET;
             epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);//修改标识符，等待下一个循环时发送数据，异步处理的精髓
         }
         else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
         {
             struct myepoll_data* md = (myepoll_data*)events[i].data.ptr;    //取数据
             sockfd = md->fd;
             send( sockfd, md->ptr, strlen((char*)md->ptr), 0 );        //发送数据
             ev.data.fd=sockfd;
             ev.events=EPOLLIN|EPOLLET;
             epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev); //修改标识符，等待下一个循环时接收数据
         }
         else
         {
             //其他的处理
         }
     }
 }

</pre>


### man 手册使用
- man man 查看man手册的使用
- man x any 查看x章节里面ang的文档

Linux进程启动的时候,在内核空间会生成一个pcb进程控制块，其中pcb中有一个一打开的文件描述符表，文件描述符表记录着
文件描述符与对应file结构体的地址[这也就是为什么操作文件用文件描述符就可以操作了]
linux进程中的文件描述符,前三个是默认被占用stdin stdout stderr
进程能够打开的进程文件是有限制的
文件描述符的个数有限制,一般是1024个[具体取决于系统]

### errno
errno是用来函数错误码的一个全局变量
不同的errno对应不同的错误信息
打印errno对应的错误信息：perror(“hello world”),输出hello world的同时打印错误信息
获取errno对应的错误信息：strerror(errno)

### 阻塞和非阻塞
阻塞和非阻塞是文件的属性
- 普通文件默认不是阻塞的 ,比如 demo.cpp

- 终端设备默认是阻塞的,可以通过设置改变属性
    1、套接字
    2、管道


shell脚本管理守护进程


### 线程池设计思路：
    生成一批线程和一个阻塞任务队列,线程从阻塞队列获取任务进行消费,客户端将任务抛到人物队列


### 文件操作符操作函数：
dup
  int dup(int oldfd);
        dup操作返回一个指向oldfd所指向的文件的fd,当关闭oldfd是，文件并不会关闭，可以继续使用返回的fd

  int dup2(int oldfd, int newfd);
        newfd指向oldfd指向的文件

fcntl
    int fcntl(int fd, int cmd, ... /* arg */ );
    操作文件描述


### fork创建进程
    fork创建进程是通过对父进程的拷贝clone. 它们进程数据区的数据一模一样，
    但是内核区的进程ID不同. 在后续的运行过程中，它们之间互不影响.创建出来的
    瞬间它们的运行位置一模一样[从fork代码之后开始运行]。

pid_t fork(void);
返回值：
    子进程返回值：0
    父进程返回值：子进程的id
getpid 获取当前进程id
getppid 获取父进程id


### 查看进程 kill命令
ps au 查看依赖终端运行的程序[TTY列显示为tty]
ps aux 查看所有的运行程序[TTY列显示为？]
ps ajx 可以显示父pid：ppid 进程组id：pgid 会话id：sid

ps aux | grep xxx

kill -l 查看系统信号
kill -9 pid 给进程为pid的进程发送SIGKILL信号[赐死信号 必死无疑]

### 父子进程之间的数据
fork出子进程的时 它们的用户区数据一模一样，随着后续代码的执行，
用户区的数据会各自操作各自的用户数据，父子进程间互不影响。父子进程用户区的数据，在发生写操作的
时候，才会发生拷贝操作[为了优化]；否则如果只有读操作，内存中只有一份数据。

### exec族函数
    可以使得父子进程执行不同的操作，本质是操作父子进程中代码段中的代码。

execl("xxx" ....)
用xxx程序替换进程中原来的代码,这么做就可以使得执行一个程序不用在分配地址空间。


### 进程回收
孤儿进程：
    父进程先结束，子进程就被称为孤儿进程。子进程被init进程领养，init进程就变成了
孤儿进程的父进程，这么做是为了能够释放子进程所占用的数据。
子进程死后会自动释放用户空间的数据，当时其无法释放pcb[内核空间的数据]，
pcb必须由子进程的父进程释放。

僵尸进程：
    子进程结束了，父进程没结束一直不释放子进程的pcb，此时子进程就成为了僵尸进程。僵尸进程
不是一个正常活着的进程，它是一个已死的进程[只是pcb没释放]

pid_t wait(int *wstatus);阻塞函数，等待回收子进程的死亡。
wstatus：
   接收进程的死因，还是被信号杀死的。
返回值： -1 表示没有了子进程
        >0 被回收的子进程呢个的id

pid_t waitpid(pid_t pid, int *wstatus, int options);
waitpid函数可以是阻塞或者非阻塞，通过options控制.可以制定回收那个子进程。
pid == -1：回收所有的子进程[循环回收]，包括被送出去给其他进程组的进程
pid == 0：回收当前进程组的所有子进程
pid > 0 :回收指定的子进程
pid == <0[子进程pid取反]，跨进程组回收子进程[被原生进程组送出来的子进程]。
options==0，阻塞
options==WNOHANG 非阻塞
返回值：-1 阻塞模式下，表示没有了子进程
       >0 被回收的子进程的pid
       如果为非阻塞
          0 子进程处于运行状态


### 进程间通信IPC
pipe[匿名管道]
    进行有血缘关系进程之间通信，父子和兄弟进程之间通信，在磁盘上没有对应的磁盘文件，
    本质就是内核的缓冲区，是一个伪文件[不占有磁盘空间]
    读写两端对应两个文件描述符 [W写端 -------- R读端]
    操作管道的进程被销毁之后，管道会被自动释放销毁
    管道默认是阻塞的
管道的底层实现方式是采用的队列的方法[环形队列],默认大小是4K，大小内核会调正，半双工[只能在一端写，另外一端读]
创建匿名管道：
    int pipe(int pipefd[2]/*传出参数*/);
        pipefd[0]:读端
        pipefd[1]:写端
    返回值：0 创建成功
          -1 创建失败


fifo[有名管道]
    可以进行无血缘关系进程间通信，通信时会在磁盘上生成文件

signal信号：系统开销小

mmap
    创建匿名映射区域，有血缘关系和无血缘关系

socket
    采用本地套接字的方式

文件：共享文件






















