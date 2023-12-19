#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cctype>

#include "utils.h"

char* get_file_content(const char* filename)
{
    assert(filename);

    FILE* file = fopen(filename, "rb");

    int file_size = get_file_size(file);

    char* buffer = (char*) calloc(file_size + 1, sizeof(char));
    assert(buffer);

    buffer[file_size] = '\0';
    fread(buffer, sizeof(char), file_size, file);

    fclose(file);

    return buffer;
}

int get_file_size(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    int position = ftell(file);
    fseek(file, 0, SEEK_SET);

    return position;
}

int calc_nlines(char* buffer)
{
    assert(buffer);

    int nlines = 0;
    char* temp = buffer;

    while ((temp = strchr(temp, '\n')) != nullptr)
    {
        nlines++;
        temp++;
    }

    return nlines;
}

int calc_symbols_in_line(char** buffer)
{
    assert(buffer);

    int shift = 0;
    while ((*buffer)[shift] != '\n')
    {
        if ((*buffer)[shift] == ';')    //skip comments
        {
            (*buffer)[shift] = '\0';
        }

        shift++;
    }

    return shift + 1;
}

char* remove_empty_lines(char* buffer, int* line, int nlines)
{
    assert(buffer);
    assert(line);

    while (*buffer == '\n' && *line < nlines)
    {
        (*line)++;
         buffer++;
    }

    if (*buffer == ';')
    {
        while (*buffer != '\n') buffer++;
        buffer++;
        (*line)++;
    }


    return buffer;
}

int skip_spaces (char* buffer)
{
    assert (buffer);

    int i = 0;

    while (isspace (buffer[i])) i++;

    return i;
}

bool is_equal(double a, double b)
{
    const double EPS = 1e-10;
    return fabs(a - b) < EPS;
}
