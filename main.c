#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>
#include "flashcard.h"
#include "csvparser.h"
#define FLASHCARDS_TO_LOAD 100

int main(int argc, char* argv[]){
    FILE * cards;
    char * line;
    size_t len = 0;
    ssize_t readline;
    struct flashcard* flashcards[FLASHCARDS_TO_LOAD];
    int loadedFlashcards = 0;
    char **cardLine;

    if(argc != 2){
        printf("Please chose an input file\n");
        exit(EXIT_FAILURE);
    }

    cards = fopen(argv[1], "r");
    if (cards == NULL)
        exit(EXIT_FAILURE);

    while ((( readline = getline(&line, &len, cards)) != -1) && (loadedFlashcards < FLASHCARDS_TO_LOAD)){
        if(line[0] == '#') continue;
        struct flashcard* flash = malloc(sizeof(struct flashcard));
        cardLine = parse_csv(line);
        flash->front = cardLine[0];
        flash->back = cardLine[1];
        flash->repetitions = strtoumax(cardLine[2], NULL, 10);
        free(cardLine[2]);
        flash->easinessFactor = strtof(cardLine[3], NULL);
        free(cardLine[3]);
        flash->dueDay = strtoumax(cardLine[4], NULL, 10);
        free(cardLine[4]);
        flashcards[loadedFlashcards] = flash;
        loadedFlashcards++;
    }
    free(line);
    fclose(cards);


    char c = '\n';
    int throwaway;
    int repeat = 0;
    int today = ((unsigned long)time(NULL))/86400; //unix time in days
    do{
        repeat = 0;
        for(int i = 0; i < loadedFlashcards; i++){
            if(flashcards[i]->dueDay <= today){
                printf("%s\n",flashcards[i]->front);
                printf("Your guess: ");
                while((throwaway = getchar()) != '\n' && throwaway != EOF);
                printf("The answer was: %s\n",flashcards[i]->back);
                do{
                    printf("How did you do (0-5): ");
                    fflush(stdout);
                    c = getchar();
                    while(c == ' ' || c == '\t') c = getchar(); //get rid of leading whitespace
                    if(c != '\n') while((throwaway = getchar()) != '\n' && throwaway != EOF); //clean up stdin
                }while(c < '0' || c > '5');
                flashcards[i]->dueDay = today + repetitionInterval(flashcards[i]->easinessFactor, flashcards[i]->repetitions, c - '0');
                if(c < '3'){
                    flashcards[i]->repetitions = 0;
                    repeat = 1;
                }else{
                    flashcards[i]->repetitions += 1;
                }
                flashcards[i]->easinessFactor = newEasinessFactor(flashcards[i]->easinessFactor,c-'0');
                if(flashcards[i]->dueDay == today)
                    printf("You'll see me again today!\n");
                else if(flashcards[i]->dueDay == today + 1)
                    printf("You'll see me again tomorrow!\n");
                else
                    printf("You'll see me in %d days!\n",flashcards[i]->dueDay - today);
                printf("\n");
            }
        }
    }while(repeat);

    mkdir("backup", (mode_t) 0755);
    char backup[100];
    sprintf(backup,"backup/%lu-%s",time(NULL),argv[1]);
    rename(argv[1],backup);

    FILE* newfile = fopen(argv[1],"w");
    for(int i = 0; i < loadedFlashcards; i++){
        fprintf(newfile,"\"%s\",\"%s\",%d,%f,%d\n",flashcards[i]->front,flashcards[i]->back,flashcards[i]->repetitions,flashcards[i]->easinessFactor,flashcards[i]->dueDay);
    }
    fclose(newfile);
    printf("\n");
    exit(EXIT_SUCCESS);
}
