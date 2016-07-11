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
#include "AirListener.h"
#include "Thread/AirThread.h"
#include "Thread/AirOperation.h"

namespace AirCpp {
    class Server {
    protected:
        Listener *_pListener;
        Thread *m_pListenThread;
        int m_iMaxHandle;
        
        std::map<int, Connection *> m_mapConnections;
        fd_set m_ConnSet;
        
        void startListen() {
            Thread::excute_async(m_pListenThread, [&] {
                AirCpp::Listener *pListener = new AirCpp::Listener(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                pListener->init(28080, 100);
                pListener->start_listen([&](const int & handle, Connection *connect) {
                    printf("handle connection");
                    m_iMaxHandle = std::max(handle, m_iMaxHandle);
                    FD_SET(handle, &m_ConnSet);
                    m_mapConnections[handle] = connect;
                });
            });
        }
        
        void read() {
            fd_set tmpSet;
            while (true) {
                FD_ZERO(&tmpSet);
                tmpSet = m_ConnSet;
                int h = ::select(m_iMaxHandle + 1, &tmpSet, NULL, NULL, NULL);
                switch (h) {
                    case -1:
                        //clear set
                        break;
                    case 0:
                        //timeout
                        break;
                    default:
                        //read
                        handleReceiveData(tmpSet);
                        continue;
                }
            }
        }
        
        void handleReceiveData(const fd_set &set) {
            for(const auto& kvp : m_mapConnections) {
                if(FD_ISSET(kvp.second->mSocket.m_iSocketHandle, &set)) {
                    kvp.second->m_fReseiveHandler();
                }
            }
        }
    public:
        Server() {
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
