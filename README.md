# EasyTcpServer
TCP服务器和客户端

1.0:使用recv send等函数进行单次收发

1.1:服务器和客户端通过while循环，一对一交互

1.2:将发送的报文进行结构化和封装，可以通过一次收发对信息进行处理

1.3：使用select模型升级服务器和客户端，使得服务器可以处理多个客户端的连接，timeval参数设置后成为非阻塞select，增加处理其他事务的能力

1.4：使用thread修改客户端，分离出一个线程用来输入命令。增加Linux版本客户端

1.5: 对客户端和服务器进行类封装 处理客户端和服务器的粘包少包问题 用标准库chrono提供的计时器来计算服务端接收速度
在socket网络模型下 连接较多时处理的延迟较高 会达到毫秒微秒 且新连接加入变慢

1.6:使用多线程分离出连接接收和消息处理线程，一个接收连接线程和几个消息接收线程之间通过一个缓冲队列来传递加入的客户端（生产者-消费者模式）

1.7：通过加入一个接口来让消息接收线程通知客户端的退出事件，并删除接收连接和接受消息函数多余的部分

目前在本地测试下，对于一万个连接，接入时间大概在十秒内，每个连接一次进行10个字节的发送，能够达到每秒六十万包的收发速度
