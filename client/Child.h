//
// Created by radu on 02.01.2019.
//

#ifndef REMMUX_CHILD_H
#define REMMUX_CHILD_H

class Child{
private:
    bool run_loop = true;
    WINDOW_DESC cwd;//tty is x+1, y+1, width-2, height-3
//    FILE *debug;

    int sd;
    sockaddr_in server;
    WINDOW *child_window = nullptr, *tty_window = nullptr;
    int screen_stdin[2];
    int input_pos = 0;
    int piped_char;
    char input_buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];

//    bool connect_to_server();
    bool stream_screen_content(
            const char *send_buf,
            char *recv_buf);

public:
    int Get_Child_Stdin();
    void Focus_Window();
    void loop();
    void Resize_Event();
//    bool check_heartbeat();
    explicit Child(int socket_descriptor);
    void Refresh_Window();
    void Redraw_Window();
    void Set_Pos_Size(WINDOW_DESC w_desc);

    // Prevent copying (important for managing sockets, pipes, ncurses windows)
    Child(const Child&) = delete;
    Child& operator=(const Child&) = delete;

    // Allow moving if needed
    Child(Child&&) = default;
    Child& operator=(Child&&) = default;
};


#endif //REMMUX_CHILD_H
