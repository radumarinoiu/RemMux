//
// Created by radu on 12/7/18.
//
#include <ncurses.h>

#ifndef REMMUX_CONSOLE_H
#define REMMUX_CONSOLE_H

struct console{
    int x, y;
    WINDOW *handle;
};
int console_main();
WINDOW *console_create(WINDOW*, int, int, int, int);
int console_delete(struct console*);
int console_move(int, int, int, int);
//int console_resize(int, int);
int console_switch(int, int);
void rebuild_screen_grid();

#endif //REMMUX_CONSOLE_H
