//
//  Listener.h
//  AirCpp
//
//  Created by Penghao on 14-2-25.
//  Copyright (c) 2014年 PengHao. All rights reserved.
//

#ifndef __AirCpp__Listener__
#define __AirCpp__Listener__

#include <iostream>
#include <map>
#include "AirConnection.h"

namespace AirCpp{
    typedef std::function< void(const int &handle, Connection *connect)> ListenerCallBack;
    class Listener {
    protected:
        unsigned int max_num_in_queue;
        unsigned long max_connection_num;
        unsigned long current_connection_num;
        bool is_starting;
        Socket mSocket;
        int m_iDomainType;
        int m_iDataType;
        int m_iProtocol;
    public:
        
        
        Listener(int domainType, int dataType, int protocol):
        m_iDomainType(domainType),
        m_iDataType(dataType),
        m_iProtocol(protocol),
        current_connection_num(0),
        max_connection_num(100),
        is_starting(false),
        max_num_in_queue(10)
        {
        }
        
        /**
         portnum, 一个监听的接口.
         backlog, 一个决定监听队列大小的整数，当有一个连接请求到来，就会进入此监听队列，当队列满后，新的连接请求会返回错误。
         */
        int init(unsigned short portnum, unsigned int backlog) {
            if (mSocket.init(m_iDomainType, m_iDataType, m_iProtocol) == 0 && mSocket.bind(portnum) == 0 && mSocket.listen(backlog) == 0) {
                return 0;
            } else {
                return -1;
            }
        }
        
        
        int start_listen(ListenerCallBack callback){
            printf("start listen socketHandle\n");
            is_starting = true;
            int  t = -1;
            sockaddr client_addr;
            socklen_t client_size;
            while (is_starting) {
                t = mSocket.accept(&client_addr, &client_size);
                if (t < 0) { /* get a connection */
                    if (errno == EINTR)             /* EINTR might happen on accept(), */
                    {
                        continue;                   /* try again */
                    }
                    else
                        return -1;
                } else {
                    Socket *s = new Socket();
                    s->init(t, &client_addr, &client_size);
                    Connection *connect = new Connection(s);
                    callback(t, connect);
                }
                sleep(1.0);
            }
            return 0;
        };
        
        void stop_listen(){
            is_starting = false;
            mSocket.close();
        }
    };
}

#endif /* defined(__AirCpp__Listener__) */
