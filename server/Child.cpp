#include <unistd.h>
#include <netinet/in.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>

#include "../common/constants.h"
#include "Child.h"
#include "Shell.h"

Child::Child(int socket_descriptor) {
    sd = socket_descriptor;
    loop();
}

void Child::loop() {
    int pid = fork();
    if(pid == 0){
        int8_t recv_cmd;
        debug = fopen("debug_server.log", "w");

        while(run_loop){
            read(sd, &recv_cmd, 1);
            switch(recv_cmd){
                case PROTOCOL_HEARTBEAT:
                    process_heartbeat();
                    break;
                case PROTOCOL_STREAM:
                    process_stream();
                    break;
                case PROTOCOL_INITIATE_SHUTDOWN:
                    process_shutdown();
                    break;
                default:
                    perror("Received invalid command");
                    break;
            }
        }
    }
}

void Child::process_shutdown() {
    run_loop = false;
}

void Child::Shutdown() {
    process_shutdown();
}

void Child::process_heartbeat() {
    int8_t prot = PROTOCOL_HEARTBEAT;
    write(sd, &prot, 1);
}

void Child::process_stream() {
    int8_t prot = PROTOCOL_STREAM;
    write(sd, &prot, 1);
    int resp_size, recv_size = 0;
    char recv_buffer[BUFFER_SIZE], resp_buffer[BUFFER_SIZE];
    bzero(&recv_buffer, BUFFER_SIZE);
    bzero(&resp_buffer, BUFFER_SIZE);

    read(sd, &recv_size, sizeof(recv_size));
    if(recv_size > 0){
        read(sd, &recv_buffer, recv_size);
        printf("DEBUG:S(%d).", recv_size);
        for(int i = 0; i < recv_size; i++)
            printf("%c.", recv_buffer[i]);
        printf("\n");
    }
    read(sh.Get_Stdout(), resp_buffer, BUFFER_SIZE);
    fprintf(debug, "Read %s\n", resp_buffer);
    fflush(debug);

    //strcat(resp_buffer, recv_buffer);
    resp_size = strlen(resp_buffer);
    fprintf(debug, "Wrote %d bytes\n", resp_size);
    fflush(debug);
    write(sd, &resp_size, sizeof(resp_size));
    if(resp_size > 0){
        fprintf(debug, "%s\n", resp_buffer);
        fflush(debug);
        write(sd, resp_buffer, resp_size);
    }
    write(sh.Get_Stdin(), recv_buffer, strlen(recv_buffer));
}