//
//  Socket.h
//  AirCpp
//
//  Created by Penghao on 14-2-25.
//  Copyright (c) 2014年 PengHao. All rights reserved.
//

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
#ifndef AirCpp_AirBSDSocket_h
#define AirCpp_AirBSDSocket_h
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
namespace AirCpp {
    
#define MAXHOSTNAME 30 //定义域名长度
#define PORTNUM 50000 /* random port number, we need something */
    class Connection;
    class Listener;
    class Server;
    /**
     *	基于BSDSocket的socket类
     */
    class Socket {
        friend Connection;
        friend Listener;
    public:
        int m_iSocketHandle;
        sockaddr_in *m_pTarget_addr;
    protected:
        
        Socket( const Socket &s ): m_iSocketHandle(s.m_iSocketHandle) {
            m_pTarget_addr = (sockaddr_in *)malloc(sizeof(sockaddr_in));
            memcpy(m_pTarget_addr, s.m_pTarget_addr, sizeof(sockaddr_in));
        }
        
        /**
         *	初始化 方法和上面一样
         *	@param 	domainType 	为创建的套接字指定协议集。 例如：
         *	AF_INET 表示IPv4网络协议
         *	AF_INET6 表示IPv6
         *	AF_UNIX 表示本地套接字（使用一个文件）
         *	@param 	dataType 	type 如下：
         *	SOCK_STREAM （可靠的面向流服务或流套接字）
         *	SOCK_DGRAM （数据报文服务或者数据报文套接字）
         *	SOCK_SEQPACKET （可靠的连续数据包服务）
         *	SOCK_RAW (在网络层之上的原始协议)
         *	@param 	protocol 	protocol 指定实际使用的传输协议。 最常见的就是IPPROTO_TCP、IPPROTO_SCTP、IPPROTO_UDP、IPPROTO_DCCP。这些协议都在<netinet/in.h>中有详细说明。 如果该项为“0”的话，即根据选定的domain和type选择使用缺省协议。
         *
         *	@return	创建成功返回0 创建失败返回-1
         */
        Socket(long timeout = 0):
        m_iSocketHandle(-1),
        m_pTarget_addr(NULL)
        {
            m_pTarget_addr = (sockaddr_in *)malloc(sizeof(sockaddr_in));
            memset(m_pTarget_addr, 0, sizeof(sockaddr_in));
            struct sigaction action;
            action.sa_handler = handle_pipe;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            sigaction(SIGPIPE, &action, NULL);
        }
        
        ~Socket(){
            close();
            free(m_pTarget_addr);
        }
        
        static void handle_pipe(int sig)
        {
            //不做任何处理即可
            perror("Some socket connection is timeout!\n");
            printf("getpid = %d\n", getpid());
        }
        
        int connect(const std::string &host, int port){
            const char *ip_addr = get_ip(host);
            if (NULL == ip_addr) {
                printf("cannot find ip from host %s ,please check your url or network! \n", host.c_str());
                return -1;
            }
            
            m_pTarget_addr->sin_addr.s_addr = inet_addr(ip_addr);
            m_pTarget_addr->sin_port = htons(port);
            bzero(&(m_pTarget_addr->sin_zero), 8);
            
            if (0 != ::connect(m_iSocketHandle, (const struct sockaddr *)(m_pTarget_addr), sizeof(const struct sockaddr))) {
                perror("failed to connect socket\n");
                ::close(m_iSocketHandle);
                return(-1);                                             /* bind address to socket */
            }
            return 0;
        }
        
        int bind(unsigned short portnum){
            hostent * h = get_host_info();
            if (h == NULL) {
                printf("failed to get gethostbyname\n");
                return -1;
            }
            m_pTarget_addr->sin_family= h->h_addrtype;                     /* this is our host address */
            m_pTarget_addr->sin_port= htons(portnum);                       /* this is our port number */

            if (0 != ::bind(m_iSocketHandle, (const struct sockaddr *)(m_pTarget_addr), sizeof(const struct sockaddr))) {
                perror("failed to bind socket\n");
                ::close(m_iSocketHandle);
                return(-1);                                             /* bind address to socket */
            }
            return 0;
        }
        
        int listen(unsigned int backlog) {
            
            if (::listen(m_iSocketHandle, backlog)<0) {
                perror("listen socket failed\n");
                return -1;
            }
            return 0;
        }
        
        
        int accept(sockaddr *client_addr, socklen_t *client_size)
        {
            printf("accept connection\n");
            int t = -1;                                                          /* socket of connection */
            if ((t = ::accept(m_iSocketHandle, client_addr, client_size)) < 0){
                t = -1;
            }
            return(t);
        }
        
        int init(int domainType, int dataType, int protocol){
            m_iSocketHandle = socket(domainType, dataType, protocol);
            if (m_iSocketHandle == -1) {
                return -1;
            }
            m_pTarget_addr->sin_family = domainType;
            return 0;
        }
    
        int init(const int socket_handle, const struct sockaddr *client_addr, const socklen_t *client_size){
            memcpy(m_pTarget_addr, client_addr, sizeof(sockaddr_in));
            m_iSocketHandle = socket_handle;
            return 0;
        }
        
        long long send(const char *c_data, long long length){
            long long dataSended = (long long)::send(m_iSocketHandle, c_data, length, 0);
            return dataSended;
        }
        
        long long read(char *c_data, long long length){
            return (long long)::read(m_iSocketHandle, c_data, length);
        }
        
        /**
         *	关闭连接
         */
        void close()
        {
            ::close(m_iSocketHandle);
        }
        
        /**
         *	获取网络的ip地址
         *
         *	@param 	_host 	网络名称、网址
         *
         *	@return	返回ip地址
         */
        const char * get_ip(const std::string & _host)
        {
            struct hostent *host = gethostbyname(_host.c_str());
            if(!host){
                herror("resolv");
                return NULL;
           }
            struct in_addr **list = (struct in_addr **)host->h_addr_list;
            return inet_ntoa(*list[0]);
        }
        
        /**
         *	获取网络的信息
         *
         *	@return	返回主机信息
         */
        hostent *get_host_info(){
            char   myname[MAXHOSTNAME+1];
            gethostname(myname, MAXHOSTNAME);               /* who are we? */
            return(gethostbyname(myname));                     /* get our address info */
        }
        
    };
}
#endif
