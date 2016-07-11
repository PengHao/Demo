//
//  Connection.h
//  AirCpp
//
//  Created by Penghao on 14-2-25.
//  Copyright (c) 2014年 PengHao. All rights reserved.
//

#ifndef __AirCpp__AirBSDSocketIO__
#define __AirCpp__AirBSDSocketIO__

#include <iostream>
#include "AirSocket.h"
typedef std::function< size_t()> ReseiveHandler;
namespace AirCpp{
    class Client;
    class Listener;
    class Server;
    class Connection {
        friend Client;
        friend Listener;
        friend Server;
    protected:
        Socket mSocket;
        int m_iDomainType;
        int m_iDataType;
        int m_iProtocol;
        ReseiveHandler m_fReseiveHandler;
        
        protected:
        void handleReceive() {
            if (m_fReseiveHandler != nullptr) {
                m_fReseiveHandler();
            }
        }
        
        Connection(int domainType, int dataType, int protocol):
        m_iDomainType(domainType),
        m_iDataType(dataType),
        m_iProtocol(protocol),
        m_fReseiveHandler(nullptr)
        {
        }
        
        Connection(const Connection & conn) {
            mSocket = conn.mSocket;
            m_iDomainType = conn.m_iDomainType;
            m_iDataType = conn.m_iDataType;
            m_iProtocol = conn.m_iProtocol;
            m_fReseiveHandler = nullptr;
        }
        
        Connection(const Socket s): mSocket(s)  {
            m_fReseiveHandler = nullptr;
        }
        
        Connection *Create(int domainType, int dataType, int protocol) {
            Connection *conn = new Connection(domainType, dataType, protocol);
            return conn;
        }
        
        int init(const std::string &host, int port) {
            int rs = mSocket.init(m_iDomainType, m_iDataType, m_iProtocol);
            if (rs == 0) {
                return mSocket.connect(host, port);
            } else {
                return rs;
            }
        }
        
    public:
        
        void setReseiveHandler(ReseiveHandler reseiveHandle) {
            m_fReseiveHandler = reseiveHandle;
        }
        
        long long send(const char *c_data, long long length) {
            return mSocket.send(c_data, length);
        }
        
        long long read(char *c_data, long long length) {
            return mSocket.read(c_data, length);
        }
    };
}

#endif /* defined(__AirCpp__Connection__) */
