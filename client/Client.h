//
// Created by radu on 02.01.2019.
//

#ifndef REMMUX_CLIENT_H
#define REMMUX_CLIENT_H

#include "Child.h"
#include <memory>


class Client{
private:
    int sd;
    sockaddr_in server;
    std::vector<std::unique_ptr<Child>> children;
    int input;
    int command_mode = 0;
    int focused_child = 0;

    void resize_event();
    void read_command();

public:
    Client(char* address,
           int port);
    void Loop();
    void Create_Child();
};


#endif //REMMUX_CLIENT_H
