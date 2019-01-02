#include <unistd.h>
#include <ncurses.h>
#include <strings.h>
#include "Shell.h"

int main(){
    printf("Starting\n");
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    timeout(250);
    sleep(1);
    bool run = true;
    char in, out[BUFFER_SIZE];
    Shell sh;
    int sh_stdin = sh.Get_Stdin(), sh_stdout = sh.Get_Stdout();
    while(run){
        in = getch();
        switch(in){
            case ERR:{
                break;
            }
            default:{
                write(sh_stdin, &in, 1);
            }
        }
        bzero(out, BUFFER_SIZE);
        read(sh.Get_Stdout(), out, BUFFER_SIZE);
        printw("%s", out);
    }
}