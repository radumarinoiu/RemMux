#ifndef REMMUX_CLIENT_H
#define REMMUX_CLIENT_H

class Child{
private:
    int sd;
    sockaddr_in server;
    char buffer[1024];
    WINDOW *parent_window, *child_window, *console_window;
    int screen_stdin[2];
    int screen_stdout[2];


    bool create_console();

    bool connect_to_server();

public:
    bool check_heartbeat();

    bool stream_screen_content(
            const char *send_buf,
            char *recv_buf);

    Child(
            char* address,
            int port,
            WINDOW *parent);

    void Draw_Call();

    void Redraw_Call();
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
