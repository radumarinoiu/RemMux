#ifndef REMMUX_SHELL_H
#define REMMUX_SHELL_H

#include <string>
#include <vector>

#include "../common/constants.h"

class Shell {
private:
    bool run_loop = true;
    std::string current_dir;
    char cmd[BUFFER_SIZE];
    char *cmd_params[128];
    std::vector<std::string> cmd_history;
    int shell_stdin[2], shell_stdout[2];
    FILE *debug;

    void loop();
    void parse_into_args();
    void execute();

public:
    Shell();
    int Get_Stdin();
    int Get_Stdout();
};


#endif //REMMUX_SHELL_H
