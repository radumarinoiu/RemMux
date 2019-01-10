//
// Created by radu on 1/10/19.
//

#include "utils.h"

int utils::istrlen(const int *str) {
    const int *s;

    for (s = str; *s; ++s)
        ;
    return (s - str);
}

void utils::istr_to_str(const int *istr, char *str) {
    int istr_length = istrlen(istr);
    for(int pos = 0; pos < istr_length; pos++){
        str[pos] = istr[pos];
    }
}