#ifndef REMMUX_SERVER_H
#define REMMUX_SERVER_H


class Child {
private:
    int sd;
    int shell_stdin[2];
    int shell_stdout[2];

    void child_loop();
    void start_shell();
    void process_stream();
    void process_heartbeat();

public:
    Child(int socket_descriptor);
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
    void Stop_Listening();
};

#endif //REMMUX_SERVER_H
