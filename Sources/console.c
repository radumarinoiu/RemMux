//
// Created by radu on 12/7/18.
//

#include <unistd.h>
#include <string.h>

#include "client.h"
#include "console.h"
#include "constants.h"
#include "server.h"

int input, cursor_x = 0, cursor_y = 0;
bool RUN = TRUE;
WINDOW *Windows[MAX_WINDOWS];
int window_count = 0;

int console_main(){
    int command_pipe_fd[2], cmd_len = 0, execute_cmd = 0;
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
                        ++cmd_len;
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
                execute_cmd = 1;
            }
        }
        if(execute_cmd){
            char cmd[512];
            read(command_pipe_fd[0], cmd, cmd_len);
            if(strcmp(cmd, "quit") == 0)
                return 0;
            else if(strcmp(cmd, "create") == 0)
                console_create_cmd();
            else if(strcmp(cmd, "delete") == 0)
                console_delete_cmd();
            else if(strcmp(cmd, "select") == 0)
                console_select_cmd();
            else{
                mvprintw(REAL_CONSOLE_H, 0, "Invalid command!");
            }
            cmd_len = 0; execute_cmd = 0;
        }
        refresh();
    }
}

void rebuild_screen_grid(){
    int rows = 0, columns = 0;
    int horizontal_fit_windows_count = CONSOLE_W/MIN_WINDOW_WIDTH;
    if(window_count > MIN_WINDOW_WIDTH)
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

void console_create_cmd(){

}
void console_delete_cmd(){

}
void console_select_cmd(){

}