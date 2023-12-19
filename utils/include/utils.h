#ifndef UTILS_H
#define UTILS_H

#include <cstdio>

char* get_file_content(const char* filename);
int   calc_nlines(char* buffer);
int   get_file_size(FILE* file);
int   calc_symbols_in_line(char** buffer);
char* remove_empty_lines(char* buffer, int* line, int nlines);
bool  is_equal(double a, double b);
int   skip_spaces (char* buffer);

#endif
