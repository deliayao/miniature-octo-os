/**
 * @file:   String.h
 * @brief:  String manipulation utilities
 */

#ifndef _STRING_
#define _STRING_

// copies a string from source to destination
// this function assumes the following:
// - destination[] is null-terminated
// - source[] is at least as large as destination[]
// this function will copy until it detects the null character in destination[]
// and append a null character to the end of source[] before returning
void strcpy(char source[], char destination[]);

// returns the length of the string
// this function assumes that source[] is null-terminated
int strlen(char source[]);

// returns 1 if character 'find' is found in char[] 'source'
// returns 0 otherwise
int strcont(char source[], char find);

#endif /* _STRING_ */
