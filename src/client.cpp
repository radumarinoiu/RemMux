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

#include "client.h"
#include "constants.h"

FILE *debug = fopen("debug.log", "w");

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
        child.Loop();
}

void Client::resize_event() {
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
        w_desc.height = CONSOLE_H / rows;
        w_desc.width = CONSOLE_W / cols;
        w_desc.y = CONSOLE_H / rows * i;
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
    cursor_y = REAL_CONSOLE_H; cursor_x = 0;
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
            if (cursor_x >= REAL_CONSOLE_W) {
                cursor_x = 1;
                move(cursor_y, cursor_x);
                clrtoeol();
            }
        }
    }while(input != '\n');
    char *cmd_part;
    cmd_part = strtok(cmd, " ");
    if(strcmp(cmd_part, "create") == 0){
        Create_Child();
    }
    else if(strcmp(cmd_part, "delete") == 0){

    }
    else if(strcmp(cmd_part, "select") == 0){
        cmd_part = strtok(NULL, " ");
        focused_child = atoi(cmd_part);
        mvprintw(cursor_y, cursor_x, "Selected %d", focused_child);
        fprintf(debug, "Selected console #%d\n", focused_child);
        fflush(debug);
    }
    move(cursor_y, 0);
    clrtoeol();
}


Child::Child(int socket_descriptor) {
    int8_t send_cmd = PROTOCOL_CREATE_SESSION;
    sd = socket_descriptor;
    write(sd, &send_cmd, 1);
    pipe(screen_stdin);
    bzero(input_buffer, BUFFER_SIZE);
    bzero(output_buffer, BUFFER_SIZE);
    fcntl( screen_stdin[PIPE_READ], F_SETFL, fcntl(screen_stdin[PIPE_READ], F_GETFL) | O_NONBLOCK);
}

void Child::Resize_Event() {
    Redraw_Window();
}

void Child::Refresh_Window() {
    refresh();
    wrefresh(child_window);
    wrefresh(tty_window);
}

void Child::Redraw_Window() {
    wclear(child_window);
    child_window = newwin(cwd.height, cwd.width, cwd.y, cwd.x);
    box(child_window, 0, 0);
    tty_window = newwin(cwd.height-2, cwd.width-2, cwd.y+1, cwd.x+1);
    scrollok(tty_window, true);
    Refresh_Window();
}

void Child::Set_Pos_Size(WINDOW_DESC w_desc) {
    cwd = w_desc;
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

void Child::Loop() {
    piped_char = 0;
    read(screen_stdin[PIPE_READ], &piped_char, 1);
    input_buffer[input_pos] = piped_char;
    switch(input_buffer[input_pos]){
        case 0:{
            stream_screen_content("", output_buffer);
            break;
        }
        case '\n':{
            stream_screen_content(input_buffer, output_buffer);
            wprintw(tty_window, "%c", input_buffer[input_pos]);
            bzero(input_buffer, BUFFER_SIZE);
            input_pos=0;
            break;
        }
        case KEY_BACKSPACE:{
            int x, y;
            getyx(tty_window, y, x);
            if(x>0)
            {
                wmove(tty_window, y, x-1);
                wprintw(tty_window, " ");
                wmove(tty_window, y, x-1);
                input_buffer[input_pos] = 0;
                input_buffer[input_pos-1] = 0;
                input_pos--;
            }
            break;
        }
        default:{
            stream_screen_content("", output_buffer);
            wprintw(tty_window, "%c", input_buffer[input_pos]);
            input_pos++;
            break;
        }

    }
    wprintw(tty_window, "%s", output_buffer);
    bzero(output_buffer, BUFFER_SIZE);
    Refresh_Window();
}

int Child::Get_Child_Stdin() {
    return screen_stdin[PIPE_WRITE];
}

//bool Child::connect_to_server() {
//    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
//        perror("[client] Error creating socket.\n");
//        return false;
//    }
//    if(connect(sd, (sockaddr*)&server, sizeof(sockaddr)) == -1){
//        perror("[client] Error connecting to server.\n");
//        return false;
//    }
//    return true;
//}
//
//bool Child::check_heartbeat() {
//    int8_t prot = PROTOCOL_HEARTBEAT;
//    if(write(sd, &prot, 1) <= 0)
//        return false;
//    prot = -1;
//    if(read(sd, &prot, 1) < 0)
//        return false;
//    if(PROTOCOL_HEARTBEAT != prot)
//        return false;
//    return true;
//}



int main() {
    char ip_address[] = "127.0.0.1";
    printf("Starting\n");
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    timeout(250);
    sleep(1);
    Client main_cl(ip_address, 8912);
    main_cl.Create_Child();
    while(1){
        main_cl.Loop();
    }

    return 0;


//    FILE *ff = fopen("Test.txt", "w");
//    while(run){
//        bzero(output, BUFFER_SIZE);
//        input[pos] = getch();
//        if(input[pos] == ERR){
//            input[pos] = 0;
//            ch.stream_screen_content("", output);
//        }
//        else{
//            switch(input[pos]){
//                case '\n':
//                    ch.stream_screen_content(input, output);
//                    wprintw(stdscr, "%c", input[pos]);
//                    bzero(input, BUFFER_SIZE);
//                    pos=0;
//                    break;
//                case 7:
//                    int x, y;
//                    getyx(stdscr, y, x);
//                    wmove(stdscr, y, x-1);
//                    wprintw(stdscr, " ");
//                    wmove(stdscr, y, x-1);
//                    input[pos] = 0;
//                    input[pos-1] = 0;
//                    pos--;
//                default:
//                    ch.stream_screen_content("", output);
//                    wprintw(stdscr, "%c", input[pos]);
//                    pos++;
//
//            }
//            if(input[pos] == '\n'){
//            }
//            else{
//                ch.stream_screen_content("", output);
//                pos++;
//            }
//            wprintw(stdscr, "%c", input[pos]);
//        }
//        fprintf(ff, "%s.", output);
//        fflush(ff);
//        wprintw(stdscr, "%s", output);
//        refresh();
//    }
}