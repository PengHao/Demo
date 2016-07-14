//
//  Server.hpp
//  Socket
//
//  Created by 彭浩 on 16/4/28.
//  Copyright © 2016年 彭浩. All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>
#include <map>
#include <vector>
#include "AirListener.h"
#include "Thread/AirThread.h"
#include "Thread/AirOperation.h"

namespace AirCpp {
    class Session {
    protected:
        long long uid;
        int handle;
        
    public:
        
    };
    class Server {
    protected:
        Listener *_pListener;
        Thread *m_pListenThread;
        unsigned short m_uiPort;
        unsigned int m_uiBackLog;
        std::map<int, Connection *> m_mapConnections;
        fd_set m_ConnSet;
        
        ~Server() {
            if (_pListener) {
                delete _pListener;
            }
            if (m_pListenThread) {
                delete m_pListenThread;
            }
            for (const auto kvp : m_mapConnections) {
                delete kvp.second;
            }
        }
        
        void startListen() {
            Thread::excute_async(m_pListenThread, [&] {
                AirCpp::Listener *pListener = new AirCpp::Listener(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (pListener->init(m_uiPort, m_uiBackLog) == 0 ) {
                    pListener->start_listen([&](const int & handle, Connection *connect) {
                        connect->setReseiveHandler([=] {
                            char c_data[10000] = {0};
                            long long size = connect->read(c_data, 10000);
                            printf("size = %lld, \n%s", size, c_data);
                            return size;
                        });
                        m_mapConnections[handle] = connect;
                    });
                } else {
                    perror("listen port failed!");
                    exit(0);
                }
            });
        }
        
        void read() {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            std::vector<int> removeConnections;
            
            while (true) {
                FD_ZERO(&m_ConnSet);
                int max = 0;
                for(const auto& kvp : m_mapConnections) {
                    if (kvp.second->m_fReseiveHandler != nullptr) {
                        FD_SET(kvp.second->m_pSocket->m_iSocketHandle, &m_ConnSet);
                        max = std::max(kvp.second->m_pSocket->m_iSocketHandle, max);
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
                        handleReceiveData();
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
        
        void handleReceiveData() {
            for(const auto& kvp : m_mapConnections) {
                if(FD_ISSET(kvp.second->m_pSocket->m_iSocketHandle, &m_ConnSet)) {
                    if (kvp.second->m_fReseiveHandler != nullptr && kvp.second->m_fReseiveHandler() == 0) {
                        kvp.second->m_fReseiveHandler = nullptr;
                    }
                }
            }
        }
        
    public:

        Server(unsigned short portnum, unsigned int backlog):
        m_uiPort(portnum),
        m_uiBackLog(backlog) {
            m_pListenThread = new Thread();
            m_pListenThread->init();
        }
        
        void run() {
            startListen();
            read();
        }
    };
}

#endif /* AirBSDServer_hpp */
