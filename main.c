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

    if(argc != 2){
        fprintf(stderr,"ERROR: no file specified\n");
        exit(EXIT_FAILURE);
    }

    FILE * cards; //The file that contains the flashcards
    cards = fopen(argv[1], "r");
    if (cards == NULL){
	    fprintf(stderr,"ERROR: Could not open file %s\n",argv[1]);
        exit(EXIT_FAILURE);
	}

    struct flashcard* flashcards[FLASHCARDS_TO_LOAD]; //flashcards in memory

	int loadedFlashcards = csv_to_flashcard(cards, flashcards, FLASHCARDS_TO_LOAD);

	if(loadedFlashcards < 1){
			fprintf(stderr,"ERROR: No flashcards loaded");
			exit(EXIT_FAILURE);
	}

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

