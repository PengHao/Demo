//
//  AirClient.hpp
//  Socket
//
//  Created by 彭浩 on 16/7/9.
//  Copyright © 2016年 彭浩. All rights reserved.
//

#ifndef AirClient_hpp
#define AirClient_hpp

#include <stdio.h>
#include <sys/time.h>
#include <map>
#include <vector>
#include "Thread/AirThread.h"
#include "AirConnection.h"
namespace AirCpp {
    class Client {
        
    protected:
        Thread *m_pThread;
        int m_iMaxHandle;
        std::map<int, Connection *> m_mapConnections;
        fd_set m_ConnSet;
        
    public:
        Connection *createConnection(const std::string &host, int port) {
            Connection *connection = new Connection(AF_INET, SOCK_STREAM, IPPROTO_IP);
            if ( connection->init(host, port) == 0 ) {
                m_iMaxHandle = std::max(connection->mSocket.m_iSocketHandle, m_iMaxHandle);
                FD_SET(connection->mSocket.m_iSocketHandle, &m_ConnSet);
                m_mapConnections[connection->mSocket.m_iSocketHandle] = connection;
                return connection;
            } else {
                delete connection;
                connection = nullptr;
            }
            return connection;
        }
        
        Client() {
            FD_ZERO(&m_ConnSet);
            m_pThread = new Thread();
            m_pThread->init();
            Thread::excute_async(m_pThread, [&] {
                read();
            });
        }
        
        ~Client() {
            
        }
        
        void read() {
            struct timeval timeout;
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            std::vector<int> removeConnections;
            while (true) {
                FD_ZERO(&m_ConnSet);
                int max = 0;
                for(const auto& kvp : m_mapConnections) {
                    if (kvp.second->m_fReseiveHandler != nullptr) {
                        FD_SET(kvp.second->mSocket.m_iSocketHandle, &m_ConnSet);
                        max = std::max(kvp.second->mSocket.m_iSocketHandle, max);
                    } else {
                        removeConnections.push_back(kvp.first);
                    }
                }
                
                int h = ::select(max + 1, &m_ConnSet, NULL, NULL, &timeout);
                switch (h) {
                    case -1:
                        //clear set
                        continue;
                    case 0:
                        //select timeout
                        for (const auto &v : removeConnections) {
                            auto con = m_mapConnections[v];
                            m_mapConnections.erase(v);
                            delete con;
                        }
                        removeConnections.clear();
                        continue;
                    default:
                        //read
                        handleReceiveData(m_ConnSet);
                        for (const auto &v : removeConnections) {
                            auto con = m_mapConnections[v];
                            m_mapConnections.erase(v);
                            delete con;
                        }
                        removeConnections.clear();
                        continue;
                }
            }
        }
        
        void handleReceiveData(const fd_set &set) {
            for(const auto& kvp : m_mapConnections) {
                if(FD_ISSET(kvp.second->mSocket.m_iSocketHandle, &m_ConnSet)) {
                    if (kvp.second->m_fReseiveHandler != nullptr && kvp.second->m_fReseiveHandler() == 0) {
                        kvp.second->m_fReseiveHandler = nullptr;
                    }
                    
                }
            }
        }
    };
}
#endif /* AirClient_hpp */
