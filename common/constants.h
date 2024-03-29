//
// Created by radu on 12/7/18.
//

#ifndef REMMUX_CONSTANTS_H
#define REMMUX_CONSTANTS_H

#define PIPE_READ 0
#define PIPE_WRITE 1

#define MAX_WINDOWS 25

#define BUFFER_SIZE 10240

#define PROTOCOL_INITIATE_SHUTDOWN 1
#define PROTOCOL_HEARTBEAT 2
#define PROTOCOL_CREATE_SESSION 3
#define PROTOCOL_STREAM 4

struct WINDOW_DESC{
    int y, x;
    int width, height;
};

#endif //REMMUX_CONSTANTS_H
