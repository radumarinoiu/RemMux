#ifndef REMMUX_CLIENT_H
#define REMMUX_CLIENT_H

#include "constants.h"

class Child{
private:
    bool run_loop = true;
    WINDOW_DESC cwd;//tty is x+1, y+1, width-2, height-3

    int sd;
    sockaddr_in server;
    WINDOW *child_window, *tty_window;
    int screen_stdin[2];
    int input_pos = 0;
    char piped_char;
    char input_buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];

//    bool connect_to_server();
    bool stream_screen_content(
            const char *send_buf,
            char *recv_buf);

public:
    int Get_Child_Stdin();
    void Loop();
    void Resize_Event();
//    bool check_heartbeat();
    Child(int socket_descriptor);
    void Refresh_Window();
    void Redraw_Window();
    void Set_Pos_Size(WINDOW_DESC w_desc);
};

class Client{
private:
    int sd;
    sockaddr_in server;
    std::vector<Child> children;
    char input_char;
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
