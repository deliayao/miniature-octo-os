/**
 * @file:   String.c
 * @brief:  String manipulation utilities
 */

#include "String.h"

int strcont(char source[], char find) {
    int count = 0;
    
	while (source[count] != '\0') {
        if (source[count] == find) {
            return 1;
        }
        count++;
    }
    return 0;
}

void strcpy(char source[], char destination[]) {
    int i;
    
    for (i = 0; source[i] != '\0'; i++) {
        destination[i] = source[i];
    }
    destination[i] = source[i]; // null-terminated
}

int strequals(char a[], char b[]) {
    int i = 0;
    
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

int strlen(char source[]) {
    int count = 0;
    
    while (source[count] != '\0') {
        count++;
    }
    
    return count;
}
