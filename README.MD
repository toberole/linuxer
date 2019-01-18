### 注意一定不要在头文件中写方法的实现

### 安装文档
sudo apt-get install glibc-doc
sudo apt-get install manpages-posix-dev

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

### 小技巧命令
<pre>
命令：ulimit -a查看缓冲区大小和支持的文件描述符数
函数：fpathconf 查看很多系统有关的信息
file xxx 查看文件xxx属性
time xxx 查看xxx程序启动到结束运行的时间,可以分析程序运行时间消耗作优化
     real_time	= user_time	+ sys_time + 损耗时间

access 可以判断文件是否存在
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


## 进程间通信IPC
### pipe[匿名管道]
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
当全部的写端关闭的时候，read会自动解除阻塞[返回0].只要还存在写端read就会阻塞。
当读端全部关闭，系统会发出SIGPIPE信号[终止进程]
读端没有全部关闭：缓冲区满了，write会被阻塞


### fifo[有名管道]
    半双工，可以进行无血缘关系进程间通信
    通信时会在磁盘上生成伪文件[ls 可以看见],磁盘空间为0.本质是伪文件映射到内核的缓冲区
创建：
    命令：mkfifo 管道名字
    函数：mkfifo

### mmap 非阻塞
    1、创建匿名映射区域，只能用于有血缘关系的进程间通信
    2、创建磁盘文件和内存之间的映射,操作内存会被系统同步到文件，有血缘关系和非血缘关系都可以
void *mmap(
    void *addr/*映射区首地址*/,
    size_t length/*映射区大小 大小不能为零，一般设置为磁盘文件的大小，最终的大小 由内核决定*/,
    int prot,/*映射区操作权限*/
    int flags,/* MAP_SHARED MAP_PRIVATE*/
    int fd,/*被映射的磁盘文件描述符 该文件的权限必须 >= port指定的权限*/
    off_t offset/* 文件指针偏移量 一般需要是4k的倍数*/);
PROT_EXEC  Pages may be executed.
PROT_READ  Pages may be read.
PROT_WRITE Pages may be written./* 必须设置读权限 */
PROT_NONE  Pages may not be accessed.
MAP_SHARED： 修改数据会同步到磁盘
MAP_PRIVATE： 修改数据不会同步到磁盘
注意：
    当open一个文件的时候，采用的是o_create[新建的文件，大小为零]，需要对文件作拓展操作lseek、truncate
RETURN VALUE
    On success, mmap() returns a pointer to the mapped area.
    On error, the value MAP_FAILED (that is, (void *) -1) is returned.
// 释放区内存
int munmap(void *addr/*映射区首地址*/,
           size_t length/*映射区大小 大小不能为零*/);

### signal信号：
    信号是由内核发送的，Linux特有的,windows上采用的是事件机制,不同的版本的信号可能不同
信号状态：
1、未决，等待被处理
2、递达，忽略、捕获、执行默认动作
信号优先级别比较的高，当进程收到信号的时候，需要优先处理信号。

SIGKILL,SIGSTOP 不能被忽略、捕获


阻塞信号集、未决信号集，存在内核pcb里面，用户不能直接操作
阻塞信号集：要屏蔽的信号
未决信号集：没有被处理的信号的集合

发送信号：
int kill(pid_t pid, int sig);给进程为pid的进程发送一个sig信号
int raise(int sig); 给自己发送一个信号
void abort(void); 给自己发送一个异常终止信号，同时可以产生core文件[记录相关信息]
unsigned int alarm(unsigned int seconds); 发送定时信号，每个进程只能有一个定时器，自然定时法
    注意：当调用alarm(5),还没到5秒的时候，调用alarm(0) 相当于终止定时信号
getitimer, setitimer - get or set value of an interval timer

发送一个信号，信号先进入到未决信号集，然后会在阻塞信号集中查找有没有该信号，
如果阻塞信号集中有该信号，那么就不会调用处理信号的函数，否则就调用处理该信号的逻辑。

信号集API
int sigemptyset(sigset_t *set);// 清空信号集合所有标志位

int sigfillset(sigset_t *set);// 填充信号集合所有标志位

int sigaddset(sigset_t *set, int signum);// 填充信号集合signum标志位

int sigdelset(sigset_t *set, int signum);// 删除信号集合signum标志位

int sigismember(const sigset_t *set, int signum);// 判断signum是否添加进set了

//  将自定义信号集设置给内核阻塞信号集
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int sigpending(sigset_t *set);// 读取当前进程的未决信号集

捕获信号:
int sigaction(
    int signum,
    const struct sigaction *act,
    struct sigaction *oldact);

// 不跨平台
sighandler_t signal(int signum/*捕捉的信号*/, sighandler_t handler/*信号处理器*/);

### socket
    采用本地套接字的方式

### 文件：共享文件


父子进程可共享的资源：
1、文件描述符
2、内存映射区

### 守护进程
    后台程序，不需要与用户交互，不受登陆和注销的影响。
多个进程组成进程组，多个进程组构成会话[会话里面的“会长”就是守护进程，创建守护进程可以等价于创建“会长”]。
进程组里面的第一个进程就是组长，组长的id就是进程组的id。
创建会话注意事项：
    1、组长不能创建会话
    2、创建会话的进程就是会长[有些系统创建会话需要权限]
    3、创建新会话会丢弃原来的会话终端
创建守护进程步骤：
    先fork -> 父进程死亡 -> 子进程执行创建会话的操作[setid()]
    1、fork 子进程，父进程退出[必须做]
    2、子进程创建新会话[必须做]
        setsid();
    3、改变当前工作组目录[改变到系统根目录]
        之所以需要改变工作组目录，是为了防止启动守护进程的目录卸载了
    4、重设文件掩码
        子进程会继承父进程的文件掩码，增加程序的灵活性umask(0)
    5、关闭文件描述符 0,1,2 进程默认使用的
    6、执行

### 文件权限
文件所有者-群组用户-其他用户
111 - 111 - 111 [RWX 读写执行]

### 线程
创建线程之后，地址空间没有变化，进程随即退化成了线程[主线程]
主线程和子线程都有各自的pcb[子线程的pcb来自于主线程]
主线程和子线程
共享：
    .text .bbs .data 堆 动态库加载区域 环境变量 命令行参数
不共享：
    栈
在linux下面线程也是进程[轻量级进程 LWP]
查看线程id:
    1、 ps aux | grep xxx 先找到进程id
    2、 ps -Lf xxx_pid

### 创建线程 C++11 std::thread
int pthread_create(
    pthread_t *thread/*传出参数 线程id*/,
    const pthread_attr_t *attr/* 线程属性 一般设置NULL*/,
    void *(*start_routine) (void *)/* 线程执行的函数 */,
    void *arg/* 线程执行是 传递给函数的参数 */)
如果设置子线程与父线程分离，那么子线程会自己回收pcb
如果没有设置分离，那么需要父线程去回收子线程的pcb[默认是不分离的 即join的]
注意： perror() 不能在线程中使用,需要使用strerror(ret)
pthread_self()// 获取线程id

void pthread_exit(void *retval/* 该参数是个传出参数，可以被pthread_join第二个参数接收 */);// 退出一条线程

int pthread_join(pthread_t thread, void **retval);// 阻塞等待子线程的结束，结束之后回收子线程的pcb

int pthread_detach(pthread_t thread);// 设置线程分离

int pthread_cancel(pthread_t thread);// 杀死一个线程，不会立即死亡[只会在取消点死亡，取消点：系统调用]
设置取消点：void pthread_testcancel(void);

### 锁
1、互斥锁 pthread_mutex_t[保护共享资源]
2、读写锁 pthread_rwlock_t，写优先级高于读优先级
3、条件变量 pthread_cond_t[本质不是锁]，用来阻塞线程，需要搭配互斥锁一起使用[比如实现生产者、消费者模型]
pthread_cond_wait: wait之后 会自动解锁释放mutex，让出cpu执行权限；
当有其他的线程发出唤醒信号[pthread_cond_signal],会与其他阻塞在同一把锁上争取锁权限。
注意：
    互斥锁主要是用来保护共享资源，条件变量是用来阻塞线程，条件变量一般需要搭配互斥锁一起使用
4、信号量 相当于高级的互斥锁[pv操作]


### 生产者消费者模型

消费者：
while(true){
    // TODO 加锁
    while(!available){// 不可消费
        // 阻塞
    }

    // TODO 消费

    // TODO 通知生产者可以生产了

    // TODO 解锁
}

生产者：
while(true){
    // TODO 加锁
    while(!available){// 不可生产
        // 阻塞
    }

    // TODO 生产

    // TODO 通知消费者可以消费了

    // TODO 解锁
}






