#include <unistd.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <poll.h>

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
        fd_set readfds;		/* multimea descriptorilor de citire */
        fd_set actfds;		/* multimea descriptorilor activi */
        struct timeval tv;		/* structura de timp pentru select() */
        int sd, client;		/* descriptori de socket */
        int optval=1; 			/* optiune folosita pentru setsockopt()*/
        int fd;			/* descriptor folosit pentru
				   parcurgerea listelor de descriptori */
        int nfds;			/* numarul maxim de descriptori */
        time_t time_now;
        time_now = time(nullptr);
        last_client_message_time = time_now;
        int8_t recv_cmd;
        debug = fopen("debug_server.log", "w");

        while(run_loop){
            time_now = time(nullptr);
            printf("%d\n", time_now - last_client_message_time);
            fflush(stdout);
            if(time_now - last_client_message_time > 5){
                printf("Client timed out, shutting down.");
                fflush(stdout);
                process_shutdown();
            }
            select()
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
    fprintf(debug, "Client timed out, shutting down.");
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