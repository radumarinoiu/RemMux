#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/wait.h>

#include "../common/constants.h"
#include "Shell.h"

Shell::Shell() {
    bzero(cmd, BUFFER_SIZE);
    pipe(shell_stdin);
    pipe(shell_stdout);
    fcntl(shell_stdout[PIPE_READ], F_SETFL, fcntl(shell_stdout[PIPE_READ], F_GETFL) | O_NONBLOCK);
    int pid = fork();
    if(pid == 0)
    {
        debug = fopen("debug.log", "w");
        fprintf(debug, "Started shell loop\n");
        fflush(debug);
        loop();
        exit(-1);
    };
    close(shell_stdin[PIPE_READ]);
    close(shell_stdout[PIPE_WRITE]);
}

int Shell::Get_Stdin() {
    return shell_stdin[PIPE_WRITE];
}

int Shell::Get_Stdout() {
    return shell_stdout[PIPE_READ];
}

void Shell::loop() {
    char in;
    do{
        read(shell_stdin[PIPE_READ], &in, 1);
        if(in == '\n'){
            fprintf(debug, "Read \\n from shell_stdin. Executing...\n");
            fflush(debug);
            std::string str(cmd);
            cmd_history.push_back(str);
            parse_into_args();
            execute();
            bzero(cmd, BUFFER_SIZE);
            for(int i = 0; i < 128; i++)
                cmd_params[i] = 0;
        }
        else if(in == 0){
            usleep(250);
        }
        else{
            fprintf(debug, "Read %c from shell_stdin.\n", in);
            fflush(debug);
            cmd[strlen(cmd)] = in;
        }
    }while(run_loop);
    close(shell_stdin[PIPE_WRITE]);
    close(shell_stdout[PIPE_READ]);
}

void Shell::parse_into_args()
{
    cmd_params[0] = strtok(cmd, " ");
    fprintf(debug, "Cmd is now %s\n", cmd_params[0]);
    fflush(debug);
    for (int i = 1; i < 128; i++) {
        cmd_params[i] = strtok(NULL, " ");
        if (cmd_params[i] == NULL)
            break;
        if (strlen(cmd_params[i]) == 0)
            i--;
    }
}

void Shell::execute() { //TODO: Must implement pipes
    fprintf(debug, "filename: %s\n", cmd_params[0]);
    fflush(debug);
    int status;
    int pid = fork();
    if(pid == 0)
    {
        dup2(shell_stdout[PIPE_WRITE], STDOUT_FILENO);
        dup2(shell_stdout[PIPE_WRITE], STDERR_FILENO);
        execvp(cmd_params[0], cmd_params);
    }
    waitpid(pid, &status, 0);
    fprintf(debug, "Finished subprocess.");
    fflush(debug);
}