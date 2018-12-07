#include <stdio.h>
#include <ncurses.h>

#include "Sources/client.h"
#include "Sources/console.h"
#include "Sources/constants.h"
#include "Sources/server.h"

int main() {
    console_main();
    getch();
    return 0;
}