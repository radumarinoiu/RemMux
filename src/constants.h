//
// Created by radu on 12/7/18.
//

#ifndef REMMUX_CONSTANTS_H
#define REMMUX_CONSTANTS_H

#define REAL_CONSOLE_H 59
#define REAL_CONSOLE_W 220
#define CONSOLE_H 59
#define CONSOLE_W 220
#define MAX_WINDOWS 25

enum MyProtocol {
    INITIATE_SHUTDOWN,
    HEARTBEAT,
    CREATE_SESSION
};

#endif //REMMUX_CONSTANTS_H
