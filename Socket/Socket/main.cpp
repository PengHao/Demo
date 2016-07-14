//
//  main.cpp
//  Socket
//
//  Created by 彭浩 on 16/4/28.
//  Copyright © 2016年 彭浩. All rights reserved.
//

#include <iostream>
#include <string.h>

#include "AirConnection.h"
#include "AirClient.hpp"
#include "AirServer.hpp"
void startConnect() {
    AirCpp::Client *c = new AirCpp::Client();
    
    AirCpp::Connection *con = c->createConnection("localhost", 28080);
//    AirCpp::Connection *con = c->createConnection("www.baidu.com", 80);
    if (con != nullptr ) {
        con->setReseiveHandler([&] {
            char c_data[10000] = {0};
            long long size = con->read(c_data, 10000);
            printf("size = %lld, \n%s", size, c_data);
            return size;
        });
        
        std::string post = "POST /index.php HTTP/1.1\r\nHost:www.baidu.com\r\nConnection : Keep-Alive\r\nCache-Control : no-cache\r\nContent-Type : text/html\r\nContent-Length : 33\r\n\r\n{\"a\":1,\"b\":2,\"c\":3,\"d\":4,\"e\":5}\r\n\r\n";
        long long size = con->send(post.c_str(), post.length());
        printf("send %lld\n", size);
    }
    int x = 0;
    std::cin>>x;
}

#include "AirListener.h"
void startListen() {
    AirCpp::Server *server = new AirCpp::Server(28080, 100);
    
    server->run();
}


int main(int argc, const char * argv[]) {
//    startListen();
    startConnect();
}