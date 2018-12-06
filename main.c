#include <stdio.h>
#include <ncurses.h>
#include "Headers/constants.h"
#include "Headers/client.h"

struct window_obj{
    int height, width, starty, startx;
    WINDOW *handle;
};

int main() {
    initscr();

    return 0;
}