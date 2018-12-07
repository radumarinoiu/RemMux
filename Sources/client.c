//
// Created by radu on 12/7/18.
//

#include "client.h"
#include "console.h"
#include "constants.h"
#include "server.h"

int client_main(){
    while(1){
        int key = getch();
        switch(key){
            case 1:
                printf("\n:");
                break;
            default:
                printf("Invalid key %d.\n", key);
        }
    }
}