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
#include <fcntl.h>

#include "client.h"
#include "constants.h"


void Client::main_loop() {
    //Dump all stdout from pipe to child screens
    //If any window moved, redraw screen
    //Render all children and screen
}

bool Client::Start_Client() {

}

int Client::Join_Client_Thread() {

}


bool Child::create_console() {

}

bool Child::connect_to_server() {
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

bool Child::check_heartbeat() {
    int8_t prot = PROTOCOL_HEARTBEAT;
    if(write(sd, &prot, 1) <= 0)
        return false;
    prot = -1;
    if(read(sd, &prot, 1) < 0)
        return false;
    if(PROTOCOL_HEARTBEAT != prot)
        return false;
    return true;
}

bool Child::stream_screen_content(const char *send_buf, char *recv_buf){ //TODO: Implement reading/writing from/to screen pipes.
    int8_t prot = PROTOCOL_STREAM;
    int8_t send_size = strlen(send_buf), recv_size;

    if(write(sd, &prot, 1) <= 0)
        return false;
    prot = -1;

    if(read(sd, &prot, 1) < 0)
        return false;

    if(PROTOCOL_STREAM != prot)
        return false;

    write(sd, &send_size, 1);

    if(send_size > 0)
        write(sd, send_buf, send_size);

    read(sd, &recv_size, 1);
    if(recv_size > 0){
        read(sd, recv_buf, recv_size);
    }
}

Child::Child(char *address, int port, WINDOW *parent) {
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_port = htons(port);
    parent_window = parent;
    connect_to_server();
    create_console();
}

void Child::Draw_Call() {

}

void Child::Redraw_Call() {
    Draw_Call();
}



int main() {
    char input[2];//[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    printf("Starting\n");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    timeout(100);
    sleep(2);
    char ip_address[] = "127.0.0.1";
    Child ch(ip_address, 8912, stdscr);
    while(1){
        //scanf("%[^\n]", input);
        bzero(input, 2);
        bzero(output, BUFFER_SIZE);
        input[0] = getch();
        if(input[0] == ERR)
            input[0] = 0;
        ch.stream_screen_content(input, output);
        wprintw(stdscr, "%s", output);
        refresh();
    }
    getchar();
}