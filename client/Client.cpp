#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ncurses.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../common/constants.h"
#include "Client.h"
#include "Child.h"

Client::Client(char *address, int port) {
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_port = htons(port);

    Create_Child();
}

void Client::Loop() {
    input_char = getch();
    switch(input_char){
        case ERR:
            break;
        case KEY_RESIZE:
            resize_event();
            break;
        case 1:
            read_command();
            break;
        default:
            write(children[focused_child].Get_Child_Stdin(), &input_char, 1);
            break;
    }
    for(Child &child: children)
        child.loop();
}

void Client::resize_event() {
    int CONSOLE_H, CONSOLE_W;
    getmaxyx(stdscr, CONSOLE_H, CONSOLE_W);
    int window_count = children.size();
    int rows = 0, cols = 0;
    int i = 0, j = 0;
    WINDOW_DESC w_desc;

    while(rows*cols < window_count)
        if(cols > rows)
            rows++;
        else
            cols++;

    erase();
    for(Child &child: children){
        if(j >= cols || i*cols + j >= window_count){
            i++; j = 0;
        }
        if(i >= rows)
            break;
        w_desc.height = (CONSOLE_H - 1) / rows;
        w_desc.width = CONSOLE_W / cols;
        w_desc.y = (CONSOLE_H - 1) / rows * i;
        w_desc.x = CONSOLE_W / cols * j;
        child.Set_Pos_Size(w_desc);
        child.Resize_Event();
        j++;
    }
}

void Client::Create_Child() {
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("[client] Error creating socket.\n");
    }
    if(connect(sd, (sockaddr*)&server, sizeof(sockaddr)) == -1){
        perror("[client] Error connecting to server.\n");
    }
    Child child(sd);
    children.push_back(child);
    resize_event();
}

void Client::read_command() {
    int cursor_y, cursor_x;
    int CONSOLE_H, CONSOLE_W;
    getmaxyx(stdscr, CONSOLE_H, CONSOLE_W);
    cursor_y = CONSOLE_H - 1; cursor_x = 0;
    mvprintw(cursor_y, cursor_x, ":"); ++cursor_x;
    move(cursor_y, cursor_x);
    clrtoeol();
    char input;
    char cmd[BUFFER_SIZE];
    int cmd_len = 0;
    do{
        input = getch();
        if(input != ERR && input != '\n') {
            cmd[cmd_len] = input;
            cmd_len++;
            mvprintw(cursor_y, cursor_x, "%c", input);
            cursor_x++;
            if (cursor_x >= CONSOLE_W) {
                cursor_x = 1;
                move(cursor_y, cursor_x);
                clrtoeol();
            }
        }
    }while(input != '\n');
    char *cmd_part;
    cmd_part = strtok(cmd, " ");
    move(cursor_y, 0);
    clrtoeol();
    move(cursor_y, 0);
    if(strcmp(cmd_part, "create") == 0){
        Create_Child();
    }
    else if(strcmp(cmd_part, "delete") == 0){

    }
    else if(strcmp(cmd_part, "select") == 0){
        cmd_part = strtok(NULL, " ");
        focused_child = atoi(cmd_part);
        mvprintw(cursor_y, cursor_x, "Selected %d", focused_child);
    }
}
