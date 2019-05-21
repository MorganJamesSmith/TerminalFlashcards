#ifndef CSVPARSER_H
#define CSVPARSER_H
#include <stdio.h>

char **parse_csv( const char *line );
void free_csv_line( char **parsed );

#endif
