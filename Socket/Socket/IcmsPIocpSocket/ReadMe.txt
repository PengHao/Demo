2.5.0.1
  2009.12.17 初始化版本
2.9.1.2
  2010.01.27
  1.修改了处理IOCP事务时，报异常问题。                By Gongzhiqian
2.9.2.3
  2010.02.10
  1.修改了IOCP开线程时传递参数为空，有异常问题        By Gongzhiqian
2010.02.22 version2.9.3.4
  1.去掉了IOCP接收数据大小的限制，修改了动态分配内存的机制   By Liaojunjie
2010.03.02 version2.9.4.5
  1.修改了IOCP接收数据后未进行有效性验证，导致在内存拷贝的时候出错的问题 By Liaojunjie
2010.03.11 version 2.9.5.6
  1.修改了当一次内存分配失败的时候后面就无法接收数据的问题 By Liaojunjie
2012.04.24 version 2.9.6.7
  1.修改了iocp监听到新的数据时，收到的包为负数时进行类型转换错误		By LiaoJunjie