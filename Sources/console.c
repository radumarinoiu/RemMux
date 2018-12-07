//
// Created by radu on 12/7/18.
//

#include <unistd.h>

#include "client.h"
#include "console.h"
#include "constants.h"
#include "server.h"

int windows_count = 0;
int input, cursor_x = 0, cursor_y = 0;
bool RUN = TRUE;
WINDOW *Windows[MAX_WINDOWS];

int console_main(){
    int command_pipe_fd[2];
    pipe(command_pipe_fd);
    for(int i = 0; i < MAX_WINDOWS; i++)
        Windows[i] = 0;
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    timeout(50);
    while(RUN){
        input = getch();
        if(input != ERR){
            if((int)input == 1)
            {
                cursor_y = REAL_CONSOLE_H; cursor_x = 0;
                mvprintw(cursor_y, cursor_x, ":"); ++cursor_x;
                move(cursor_y, cursor_x);
                clrtoeol();
                while(input != '\n'){
                    input = getch();
                    if(input != ERR && input != '\n') {
                        write(command_pipe_fd[1], &input, 1);
                        mvprintw(cursor_y, cursor_x, "%c", input);
                        ++cursor_x;
                        if (cursor_x >= REAL_CONSOLE_W) {
                            cursor_x = 1;
                            move(cursor_y, cursor_x);
                            clrtoeol();
                        }
                    }
                }
                move(cursor_y, 0);
                clrtoeol();
            }
        }
        refresh();
    }
}

void rebuild_screen_grid(){

    refresh();
}

WINDOW *console_create(WINDOW *con, int height, int width, int starty, int startx){
    if(con != NULL){
        mvwin(con, starty, startx);
        wresize(con, height, width);
        box(con, 0, 0);
        wrefresh(con);
    }
    else{
        con = newwin(height, width, starty, startx);
        box(con, 0, 0);
        wrefresh(con);
    }
    return con;
}