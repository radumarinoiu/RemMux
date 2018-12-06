#include "Headers/client.h"

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