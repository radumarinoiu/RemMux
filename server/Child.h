//
// Created by radu on 02.01.2019.
//

#ifndef REMMUX_CHILD_H
#define REMMUX_CHILD_H

#include "Shell.h"


class Child {
private:
    int sd, pid;
    int shell_stdin[2];
    int shell_stdout[2];
    bool run_loop = true;
    Shell sh;
    FILE *debug;
    time_t last_client_message_time;

    void loop();
    void process_stream();
    void process_heartbeat();
    void process_shutdown();

public:
    Child(int socket_descriptor);
    void Shutdown();
};


#endif //REMMUX_CHILD_H
