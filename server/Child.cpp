#include <unistd.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/wait.h>

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
        int8_t recv_cmd;
        debug = fopen("debug_server.log", "w");

        while(run_loop){
            read(sd, &recv_cmd, sizeof(recv_cmd));
            switch(recv_cmd){
                case PROTOCOL_INITIATE_SHUTDOWN:
                    process_shutdown();
                    break;
                case PROTOCOL_HEARTBEAT:
                    process_heartbeat();
                    break;
                case PROTOCOL_STREAM:
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
    }
}

void Child::Shutdown() {
    process_shutdown();
}

void Child::process_heartbeat() {
    int8_t prot = PROTOCOL_HEARTBEAT;
    write(sd, &prot, sizeof(prot));
}

void Child::process_stream() {//TODO: Implement keepalive and connection timeout on server
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
    fprintf(debug, "Read %s\n", resp_buf);
    fflush(debug);

    //strcat(resp_buffer, recv_buffer);
    resp_size = strlen(resp_buf);
    fprintf(debug, "Wrote %d bytes\n", resp_size);
    fflush(debug);
    write(sd, &resp_size, sizeof(resp_size));
    if(resp_size > 0){

        fprintf(debug, "%s\n", resp_buf);
        fflush(debug);
        write(sd, resp_buf, resp_size);
    }
    write(sh.Get_Stdin(), recv_buf, strlen(recv_buf)*sizeof(recv_buf[0]));
}