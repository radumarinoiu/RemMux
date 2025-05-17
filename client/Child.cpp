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
#include "../common/utils.h"
#include "Child.h"

Child::Child(int socket_descriptor) {
    int8_t send_cmd = PROTOCOL_CREATE_SESSION;
    sd = socket_descriptor;
    write(sd, &send_cmd, sizeof(send_cmd));
    pipe(screen_stdin);
    bzero(input_buffer, BUFFER_SIZE);
    bzero(output_buffer, BUFFER_SIZE);
    fcntl(screen_stdin[PIPE_READ], F_SETFL, fcntl(screen_stdin[PIPE_READ], F_GETFL) | O_NONBLOCK);
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
    if (child_window != nullptr)
    {
        wclear(child_window);
        delwin(child_window);
    }
    child_window = newwin(cwd.height, cwd.width, cwd.y, cwd.x);
    box(child_window, 0, 0);
    if (tty_window != nullptr)
    {
        wclear(tty_window);
        delwin(tty_window);
    }
    tty_window = newwin(cwd.height-2, cwd.width-2, cwd.y+1, cwd.x+1);
    scrollok(tty_window, true);
    Refresh_Window();
}

void Child::Set_Pos_Size(WINDOW_DESC w_desc) {
    cwd = w_desc;
}

int Child::stream_screen_content(const char *send_buf, char *recv_buf){
    int8_t prot = PROTOCOL_STREAM;
    int send_size = strlen(send_buf), recv_size;

    if(write(sd, &prot, sizeof(prot)) <= 0)
        return -1;
    prot = -1;

    if(read(sd, &prot, sizeof(prot)) < 0)
        return -1;

    if(PROTOCOL_STREAM != prot)
        return -1;

    write(sd, &send_size, sizeof(send_size));

    if(send_size > 0)
        write(sd, send_buf, send_size);

    read(sd, &recv_size, sizeof(recv_size));
    if(recv_size > 0){
        read(sd, recv_buf, recv_size*sizeof(recv_buf[0]));
    }
    return recv_size;
}

void Child::loop() {
    piped_char = 0;
    read(screen_stdin[PIPE_READ], &piped_char, sizeof(piped_char));
    input_buffer[input_pos] = piped_char;
    switch(piped_char){
        case 0:{
            this->stream_screen_content(input_buffer, output_buffer);
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
        case 32 ... 126:{ //WARNING: This is a gcc extension
            stream_screen_content("", output_buffer);
            wprintw(tty_window, "%c", input_buffer[input_pos]);
            input_pos++;
            break;
        }
        default:{
            stream_screen_content("", output_buffer);
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

void Child::Focus_Window() {
    int x, y;
    getyx(tty_window, y, x);
    move(cwd.y + y + 1, cwd.x + x + 1);
}