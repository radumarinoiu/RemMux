#ifndef REMMUX_CLIENT_H
#define REMMUX_CLIENT_H

#include "constants.h"

class Child{
private:
    bool run_loop = true;
    WINDOW_DESC cwd;//tty is x+1, y+1, width-2, height-3

    int sd;
    sockaddr_in server;
    WINDOW *parent_window, *child_window, *tty_window;
    int screen_stdin[2];
    int screen_stdout[2];
    int input_pos;
    char input_buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];

    bool create_console();

    bool connect_to_server();

    void child_loop();

    void resize_event();

public:
    bool check_heartbeat();

    bool stream_screen_content(
            const char *send_buf,
            char *recv_buf);

    Child(
            char* address,
            int port,
            WINDOW *parent,
            WINDOW_DESC w_desc);

    void Refresh_Window();

    void Redraw_Window();
};

class Client{
private:
    std::list<Child> children;

    void main_loop();
public:
    bool Start_Client();
    int Join_Client_Thread();
};

#endif //REMMUX_CLIENT_H
