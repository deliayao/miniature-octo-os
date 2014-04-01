/**
 * @file:   String.h
 * @brief:  String manipulation utilities
 */

#ifndef _STRING_
#define _STRING_

/**
 * Searches for a character in a given string.
 * 
 * @param   source The string in which to search.
 * @param   find The character to find.
 * @return  1 if find is found in source, 0 otherwise.
 */
int strcont(char source[], char find);

/**
 * Copies a string from one array to another. This function assumes that the
 * source string is null-terminated.
 * 
 * @param   source The string containing the text to copy.
 * @param   destination The string to copy into.
 */
void strcpy(char source[], char destination[]);

/**
 * Checks whether two strings are equal. This function assumes that both
 * strings are null-terminated.
 * 
 * @param   a The first string.
 * @param   b The second string.
 * @return  1 if a is exactly equal to b, 0 otherwise.
 */
int strequals(char a[], char b[]);

/**
 * Gets the length of a given null-terminated string.
 * 
 * @param   source The given string.
 * @return  The length of the string.
 */
int strlen(char source[]);

#endif /* _STRING_ */
