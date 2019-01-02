#include <unistd.h>
#include <netinet/in.h>
#include <list>
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <fcntl.h>

#include "../common/constants.h"
#include "Server.h"

int main(){
    Server server(8912);
    server.Start_Listening();
}