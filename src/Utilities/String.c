/**
 * @file:   String.c
 * @brief:  String manipulation utilities
 */

#include "String.h"

void strcpy(char source[], char destination[]) {
    int i;
    
    for (i = 0; source[i] != '\0'; i++) {
        destination[i] = source[i];
    }
    destination[i] = source[i]; // null-terminated
}

int strlen(char source[]) {
    int count = 0;
    
    while (source[count] != '\0') {
        count++;
    }
    
    return count;
}
