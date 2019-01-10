#include <unistd.h>
#include <netinet/in.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "../common/constants.h"
#include "Server.h"

void child_death_handler(int sig_nr){
    int retval;
    wait(&retval);
}

int main(){
    signal(SIGCHLD, child_death_handler);
    Server server(8912);
    server.Start_Listening();
    sleep(10);
    getchar();
}