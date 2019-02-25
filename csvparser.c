#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "flashcard.h"
#include "csvparser.h"

int csv_to_flashcard(FILE *csv, struct flashcard* flashcards[], int flashcardsToLoad){

    char * line; //Stores the line currently being read
    char **lineItems; //parsed line into a list of strings
    size_t len = 0;
    ssize_t readline; //The length of string returned
    int loadedFlashcards = 0; // the number of flashcards loaded


    while ((( readline = getline(&line, &len, csv)) != -1) && (loadedFlashcards < flashcardsToLoad)){
        struct flashcard* flash = malloc(sizeof(struct flashcard));
        lineItems = parse_csv(line);
        flash->front = lineItems[0];
        flash->back = lineItems[1];
        flash->repetitions = strtoumax(lineItems[2], NULL, 10);
        free(lineItems[2]);
        flash->easinessFactor = strtof(lineItems[3], NULL);
        free(lineItems[3]);
        flash->dueDay = strtoumax(lineItems[4], NULL, 10);
        free(lineItems[4]);
        flashcards[loadedFlashcards] = flash;
        loadedFlashcards++;
    }
    if(loadedFlashcards == 0){
            fprintf(stderr,"ERROR: file is empty\n");
            exit(EXIT_FAILURE);
    }
    free(line);
    return loadedFlashcards;
}
void free_csv_line(char **parsed)
{
    char **ptr;
    for (ptr = parsed; *ptr; ptr++){
        free(*ptr);
    }
    free(parsed);
}

static int count_fields(const char *line)
{
    const char *ptr;
    int cnt, fQuote;
    for (cnt = 1, fQuote = 0, ptr = line; *ptr; ptr++){
        if (fQuote){
            if (*ptr == '\"'){
                if (ptr[1] == '\"'){
                    ptr++;
                    continue;
                }
                fQuote = 0;
            }
            continue;
        }
        switch(*ptr){
            case '\"':
                fQuote = 1;
                continue;
            case ',':
                cnt++;
                continue;
            default:
                continue;
        }
    }
    if (fQuote){
        return -1;
    }

    return cnt;
}

/*
 *  Given a string containing no linebreaks, or containing line breaks
 *  which are escaped by "double quotes", extract a NULL-terminated
 *  array of strings, one for every cell in the row.
 */
char **parse_csv(const char *line){
    char **buf, **bptr, *tmp, *tptr;
    const char *ptr;
    int fieldcnt, fQuote, fEnd;
    fieldcnt = count_fields(line);

    if (fieldcnt == -1){
        return NULL;
    }

    buf = malloc(sizeof(char*) * (fieldcnt+1));

    if (!buf){
        return NULL;
    }

    tmp = malloc(strlen(line) + 1);

    if (!tmp){
        free(buf);
        return NULL;
    }

    bptr = buf;

    for (ptr = line, fQuote = 0, *tmp = '\0', tptr = tmp, fEnd = 0; ; ptr++){
        if (fQuote){
            if (!*ptr){
                break;
            }
            if (*ptr == '\"'){
                if (ptr[1] == '\"'){
                    *tptr++ = '\"';
                    ptr++;
                    continue;
                }
                fQuote = 0;
            }
            else *tptr++ = *ptr;
            continue;
        }
        switch(*ptr){
            case '\"':
                fQuote = 1;
                continue;
            case '\0':
                fEnd = 1;
            case ',':
                *tptr = '\0';
                *bptr = strdup(tmp);

                if (!*bptr){
                    for (bptr--; bptr >= buf; bptr--){
                        free(*bptr);
                    }
                    free(buf);
                    free(tmp);

                    return NULL;
                }

                bptr++;
                tptr = tmp;

                if (fEnd){
                  break;
                }else
                  continue;
            default:
                *tptr++ = *ptr;
                continue;
        }
        if (fEnd)
            break;
    }
    *bptr = NULL;
    free(tmp);
    return buf;
}
