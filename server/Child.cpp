#include <unistd.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <poll.h>
#include <time.h>

#include "../common/constants.h"
#include "Child.h"
#include "Shell.h"

Child::Child(int socket_descriptor) {
    sd = socket_descriptor;
    loop();
}

void Child::loop() {
    pid = fork();
    if(pid == 0){
        struct pollfd fd;
        int ret;
        fd.fd = sd;
        fd.events = POLLIN;
        time_t time_now;
        time_now = time(nullptr);
        last_client_message_time = time_now;
        int8_t recv_cmd;

        while(run_loop){
            time_now = time(nullptr);
            fflush(stdout);
            if(time_now - last_client_message_time > 15){
                printf("Client timed out, shutting down.");
                fflush(stdout);
                process_shutdown();
            }
            ret = poll(&fd, 1, 1000);
            if (ret <= 0)
                continue;
            read(sd, &recv_cmd, sizeof(recv_cmd));
            switch(recv_cmd){
                case PROTOCOL_INITIATE_SHUTDOWN: //Not used
                    process_shutdown();
                    break;
                case PROTOCOL_HEARTBEAT: //Not used
                    process_heartbeat();
                    break;
                case PROTOCOL_STREAM: //TODO: Implement connection timeout on server
                    last_client_message_time = time_now;
                    process_stream();
                    break;
                default:
                    break;
            }
        }
    }
}

void Child::process_shutdown() {
    run_loop = false;
    int status;
    if(pid){
        kill(pid, SIGINT);
        waitpid(pid, &status, 0);
    } else{
        _exit(0);
    }
}

void Child::Shutdown() {
    process_shutdown();
}

void Child::process_heartbeat() {
    int8_t prot = PROTOCOL_HEARTBEAT;
    write(sd, &prot, sizeof(prot));
}

void Child::process_stream() {
    int8_t prot = PROTOCOL_STREAM;
    write(sd, &prot, sizeof(prot));
    int resp_size, recv_size = 0;
    char recv_buf[BUFFER_SIZE], resp_buf[BUFFER_SIZE];
    bzero(&recv_buf, BUFFER_SIZE);
    bzero(&resp_buf, BUFFER_SIZE);

    read(sd, &recv_size, sizeof(recv_size));
    if(recv_size > 0){
        read(sd, &recv_buf, recv_size*sizeof(recv_buf[0]));
        printf("DEBUG:S(%d).", recv_size);
        for(int i = 0; i < recv_size; i++)
            printf("%c.", recv_buf[i]);
        printf("\n");
    }
    read(sh.Get_Stdout(), resp_buf, BUFFER_SIZE*sizeof(resp_buf[0]));

    //strcat(resp_buffer, recv_buffer);
    resp_size = strlen(resp_buf);
    write(sd, &resp_size, sizeof(resp_size));
    if(resp_size > 0){

        write(sd, resp_buf, resp_size);
    }
    write(sh.Get_Stdin(), recv_buf, strlen(recv_buf)*sizeof(recv_buf[0]));
}