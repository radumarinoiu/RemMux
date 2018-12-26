#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client.h"

class Client{
private:
    std::list<Child> children;

    void main_loop(){
        //Dump all stdout from pipe to child screens
        //If any window moved, redraw screen
        //Render all children and screen
    }
public:
    bool Start_Client(){

    }
    int Join_Client_Thread(){

    }
};

class Child{
private:
    int sd;
    sockaddr_in server;
    char buffer[1024];
    WINDOW *parent_window, *child_window, *console_window;
    int screen_buffer_fd[2];


    bool create_console(){

    }

    bool connect_to_server(){
        if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            perror("[client] Error creating socket.\n");
            return false;
        }
        if(connect(sd, (sockaddr*)&server, sizeof(sockaddr)) == -1){
            perror("[client] Error connecting to server.\n");
            return false;
        }
        return true;
    }

    bool check_heartbeat(){
        return true;
    }

    ssize_t read_from_socket(int fd, void *buf, size_t nbytes){
        if(!check_heartbeat())
            return -1;
        return read(fd, buf, nbytes);
    }

    ssize_t write_to_socket(int fd, const void *buf, size_t nbytes){
        if(!check_heartbeat())
            return -1;
        return write(fd, buf, nbytes);
    }

public:
    Child(char* address, int port, WINDOW *parent){
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(address);
        server.sin_port = htons(port);
        parent_window = parent;
        connect_to_server();
        create_console();
    }

    void Draw_Call(){

    }

    void Redraw_Call(){
        Draw_Call();
    }
};