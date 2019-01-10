#include <unistd.h>
#include <netinet/in.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>

#include "../common/constants.h"
#include "Server.h"
#include "Child.h"
#include "Shell.h"

Server::Server(int port) {
    bzero (&listener, sizeof (listener));
    bzero (&client, sizeof (client));
    listener.sin_family = AF_INET;
    listener.sin_addr.s_addr = htonl (INADDR_ANY);
    listener.sin_port = htons (port);
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Failed to create socket!");
        return;
    }
    if (bind (sd, (struct sockaddr *) &listener, sizeof (sockaddr)) == -1)
    {
        perror("Failed to bind to port!");
        return;
    }
}

bool Server::Start_Listening() {
    bool run = true;
    int8_t recv_cmd;
    int child_sd;
    if (listen (sd, 5) == -1)
    {
        perror("Failed to start listening!");
        return false;
    }
    while(run){
        child_sd = accept (sd, (sockaddr *) &client, &client_size);
        perror("New connection");
        if(child_sd < 0){
            perror("An error occurred accepting a connection!");
            continue;
        }
        read(child_sd, &recv_cmd, sizeof(recv_cmd));
        switch (recv_cmd){
            case PROTOCOL_CREATE_SESSION:{
                perror("New child");
                Child child(child_sd);
                children.push_back(child);
                break;
            }
            case PROTOCOL_INITIATE_SHUTDOWN:{
                for(auto child: children){
                    child.Shutdown();
                }
                run = false;
                break;
            }
            default:{
                break;
            }
        }
    }
    return true;
}