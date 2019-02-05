#ifndef CSVPARSER_H
#define CSVPARSER_H

char **parse_csv( const char *line );
void free_csv_line( char **parsed );
int csv_to_flashcard(FILE *csv, struct flashcard* flashcards[], int flashcardsToLoad);

#endif
