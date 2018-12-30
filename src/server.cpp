#include <unistd.h>
#include <netinet/in.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>

#include "server.h"
#include "constants.h"

Child::Child(int socket_descriptor) {
    sd = socket_descriptor;
    if(pipe(shell_stdin) == -1 || pipe(shell_stdout) == -1)
    {
        perror("Failed to create pipe");
        return;
    }
    child_loop();
    start_shell();
}
void Child::start_shell() {
    int pid = fork();
    if(pid == 0)
    {
        dup2(shell_stdin[PIPE_READ], STDIN_FILENO);
        close(shell_stdin[PIPE_WRITE]);
        dup2(shell_stdout[PIPE_WRITE], STDOUT_FILENO);
        dup2(shell_stdout[PIPE_WRITE], STDERR_FILENO);
        close(shell_stdout[PIPE_READ]);
        execlp("/bin/sh", "sh", NULL);
        perror("Exec failed");
        _exit(-1);
    }
    fcntl(shell_stdout[PIPE_READ], F_SETFL, O_NONBLOCK);
    close(shell_stdin[PIPE_READ]);
    close(shell_stdout[PIPE_WRITE]);
    /*
    int seeker = 0;
    char cmd[BUFFER_SIZE];
    bzero(cmd, BUFFER_SIZE);
    size_t cmd_len = 0;
    char output[BUFFER_SIZE];
    bzero(output, BUFFER_SIZE);
    printf("Starting process...\n");
    while(1){
        cmd[seeker] = getchar();
        cmd[seeker+1] = 0;
        if(cmd[seeker] == '\n'){
            printf("Sending command: .%s.\n", cmd);
            write(shell_stdin[1], cmd, strlen(cmd));
            bzero(cmd, BUFFER_SIZE);
            printf("Reading from shell\n");
            read(shell_stdout[0], output, BUFFER_SIZE);
            printf("Received: %s\n", output);
            bzero(output, BUFFER_SIZE);
            seeker = 0;
        }
        else
            seeker++;
    }
    */
}



void Child::child_loop() {
    int pid = fork();
    if(pid == 0){
        int8_t recv_cmd;
        bool run = true;

        while(run){
            read(sd, &recv_cmd, 1);
            switch(recv_cmd){
                case PROTOCOL_HEARTBEAT:
                    process_heartbeat();
                    break;
                case PROTOCOL_STREAM:
                    process_stream();
                    break;
                case PROTOCOL_INITIATE_SHUTDOWN:
                    run = false;
                    break;
                default:
                    perror("Received invalid command");
                    break;
            }
        }
    }
}

void Child::process_heartbeat() {
    int8_t prot = PROTOCOL_HEARTBEAT;
    write(sd, &prot, 1);
}

void Child::process_stream() {
    int8_t prot = PROTOCOL_STREAM;
    write(sd, &prot, 1);
    int8_t resp_size, recv_size = 0;
    char recv_buffer[BUFFER_SIZE], resp_buffer[BUFFER_SIZE];
    bzero(&recv_buffer, BUFFER_SIZE);
    bzero(&resp_buffer, BUFFER_SIZE);

    read(sd, &recv_size, sizeof(size_t));
    if(recv_size > 0){
        read(sd, &recv_buffer, recv_size);
        printf("DEBUG:S(%d).", recv_size);
        for(int i = 0; i < recv_size; i++)
            printf("%c.", recv_buffer[i]);
        printf("\n");
    }
    if(recv_size == 0)
        printf("Received refresh request.\n");
    read(shell_stdout[PIPE_READ], resp_buffer, BUFFER_SIZE);
    //strcat(resp_buffer, recv_buffer);
    resp_size = strlen(resp_buffer);
    write(sd, &resp_size, sizeof(resp_size));
    if(resp_size > 0){
        write(sd, resp_buffer, resp_size);
    }
    write(shell_stdin[PIPE_WRITE], recv_buffer, strlen(recv_buffer));
}


Server::Server(int port) {
    bzero (&listener, sizeof (listener));
    bzero (&client, sizeof (client));
    listener.sin_family = AF_INET;
    listener.sin_addr.s_addr = htonl (INADDR_ANY);
    listener.sin_port = htons (port);
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Failed to create socket!");
        return;
    }
    if (bind (sd, (struct sockaddr *) &listener, sizeof (sockaddr)) == -1)
    {
        perror("Failed to bind to port!");
        return;
    }
}

bool Server::Start_Listening() {
    if (listen (sd, 5) == -1)
    {
        perror("Failed to start listening!");
        return false;
    }
    int child_sd;
    while(1){
        child_sd = accept (sd, (sockaddr *) &client, &client_size);
        if(child_sd < 0){
            perror("An error occurred accepting a connection!");
            continue;
        }
        Child child(child_sd);
        children.push_back(child);
    }
    return true;
}

void Server::Stop_Listening() {

}



int main(){
    Server server(8912);
    server.Start_Listening();
}