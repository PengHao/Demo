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
        Socket *m_pSocket;
        int m_iDomainType;
        int m_iDataType;
        int m_iProtocol;
        ReseiveHandler m_fReseiveHandler;
        
        protected:
        ~Connection() {
            delete m_pSocket;
        }
        
        void handleReceive() {
            if (m_fReseiveHandler != nullptr) {
                m_fReseiveHandler();
            }
        }
        
        Connection(int domainType, int dataType, int protocol):
        m_iDomainType(domainType),
        m_iDataType(dataType),
        m_iProtocol(protocol),
        m_fReseiveHandler(nullptr),
        m_pSocket(nullptr)
        {
        }
        
        
        Connection(Socket *ps):
        m_fReseiveHandler(nullptr),
        m_pSocket(ps)
        {
            m_fReseiveHandler = nullptr;
        }
        
        Connection *Create(int domainType, int dataType, int protocol) {
            Connection *conn = new Connection(domainType, dataType, protocol);
            return conn;
        }
        
        int init(const std::string &host, int port) {
            m_pSocket = new Socket();
            int rs = m_pSocket->init(m_iDomainType, m_iDataType, m_iProtocol);
            if (rs == 0) {
                return m_pSocket->connect(host, port);
            } else {
                return rs;
            }
        }
        
    public:
        
        void setReseiveHandler(ReseiveHandler reseiveHandle) {
            m_fReseiveHandler = reseiveHandle;
        }
        
        long long send(const char *c_data, long long length) {
            return m_pSocket->send(c_data, length);
        }
        
        long long read(char *c_data, long long length) {
            return m_pSocket->read(c_data, length);
        }
    };
}

#endif /* defined(__AirCpp__Connection__) */
