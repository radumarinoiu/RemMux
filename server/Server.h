//
// Created by radu on 02.01.2019.
//

#ifndef REMMUX_SERVER_H
#define REMMUX_SERVER_H

#include "Child.h"


class Server {
private:
    int sd;
    sockaddr_in listener, client;
    socklen_t client_size = sizeof(client);
    std::list<Child> children;

public:
    Server(int port);
    bool Start_Listening();
};


#endif //REMMUX_SERVER_H
