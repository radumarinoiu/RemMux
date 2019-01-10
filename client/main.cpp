#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ncurses.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "../common/constants.h"
#include "Client.h"

int main() {
    char ip_address[] = "127.0.0.1";
    printf("Starting\n");
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    timeout(250);
    sleep(1);
    Client main_cl(ip_address, 8912);
    while(1){
        main_cl.Loop();
    }

    return 0;
}