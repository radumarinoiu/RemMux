#ifndef REMMUX_SERVER_H
#define REMMUX_SERVER_H


class Child {
private:
    int sd;
    int shell_stdin[2];
    int shell_stdout[2];
    bool run_loop = true;

    void Loop();
    void start_shell();
    void process_stream();
    void process_heartbeat();
    void process_shutdown();

public:
    Child(int socket_descriptor);
    void Shutdown();
};

class Server {
private:
    int sd;
    sockaddr_in listener, client;
    socklen_t client_size = sizeof(client);
    std::list<Child> children;

public:
    Server(int port);
    bool Start_Listening();
};

#endif //REMMUX_SERVER_H
