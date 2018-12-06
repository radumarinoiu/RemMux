#include <ncurses.h>

struct console{
    int height, width, starty, startx;
    WINDOW *handle;
};
int console_main();
struct console console_create();
int console_delete(struct console*);
int console_move(struct console*, int direction);
int console_resize(struct console*);
int console_switch(struct console*);