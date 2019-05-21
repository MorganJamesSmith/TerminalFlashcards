#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>
#include "csvparser.h"
#define FLASHCARDS_TO_LOAD 100

struct flashcard {
	char *front;
	char *back;
	int repetitions;
	float easinessFactor;
	int dueDay;
	int lineNumber;
};

static int repetitionInterval(float easinessFactor, int repetitions, int score);
static float newEasinessFactor(float oldEasinessFactor, int score);
static int csv_to_flashcard(FILE *csv, struct flashcard flashcards[], int flashcardsToLoad);
static void save_and_quit(char filename[], struct flashcard flashcards[], int loadedFlashcards);
static void print_user_stats(time_t start, time_t end, int score_histogram[]);
static int csv_to_flashcard(FILE *csv, struct flashcard flashcards[], int flashcardsToLoad);

int
repetitionInterval(float easinessFactor, int repetitions, int score)
{
	if(easinessFactor < 1.3) easinessFactor = 2.5;
	if(score < 3) return 0;
	int interval = 1;
	if(repetitions == 2) interval = 6;
	else if(repetitions > 2) interval = repetitionInterval(easinessFactor, repetitions-1,score) * easinessFactor;
	return interval;
}

float
newEasinessFactor(float oldEasinessFactor, int score)
{
	if(oldEasinessFactor < 1.3) return 2.5;
	float easinessFactor = (oldEasinessFactor)+(0.1-(5-score)*(0.08+(5-score)*0.02));
	if(easinessFactor < 1.3)
		easinessFactor = 1.3;
	return easinessFactor;
}

unsigned
gcd(unsigned a, unsigned b)
{
	if (b)
		return gcd(b, a % b);
	else
	return a;
}

void
save_and_quit(char filename[], struct flashcard flashcards[], int loadedFlashcards)
{
	struct timespec start,end;
	printf("\n\nSaving your progress!\n");
	clock_gettime(CLOCK_REALTIME,&start);
	mkdir("backup", (mode_t) 0755);
	char backup[100];
	sprintf(backup,"backup/%lu-%s",start.tv_sec,filename);

	printf("Moving file %s to %s\n", filename, backup);
	rename(filename,backup);

	printf("Creating new file %s\n", filename);
	FILE *newfile = fopen(filename,"w");
	printf("Populating new file %s\n", filename);
	for(int i = 0; i < loadedFlashcards; i++) {
		fprintf(newfile,"\"%s\",\"%s\",%d,%f,%d\n",flashcards[i].front,flashcards[i].back,flashcards[i].repetitions,flashcards[i].easinessFactor,flashcards[i].dueDay);
	}
	fclose(newfile);
	printf("File %s created\n\n", filename);
	clock_gettime(CLOCK_REALTIME,&end);
	printf("Saving took %ld seconds and %ld nanoseconds\n\n", end.tv_sec - start.tv_sec,end.tv_nsec-start.tv_nsec);
	exit(EXIT_SUCCESS);

}

void
print_user_stats(time_t start, time_t end, int score_histogram[])
{
	printf("\n\nYou spent %ld seconds on your flashcards\n\n", end - start);
	printf("Question Scores:\n\n");
	for(int i = 0; i < 6; i++) {
		printf("You answered %d question(s) with a score of %d!\n",score_histogram[i],i);
	}
}

int
csv_to_flashcard(FILE *csv, struct flashcard flashcards[], int flashcardsToLoad)
{

	char *line; //Stores the line currently being read
	char **lineItems; //parsed line into a list of strings
	size_t len = 0;
	ssize_t readline; //The length of string returned
	int loadedFlashcards = 0; // the number of flashcards loaded


	while ((( readline = getline(&line, &len, csv)) != -1) && (loadedFlashcards < flashcardsToLoad)) {
		struct flashcard flash;
		lineItems = parse_csv(line);
		if(lineItems == NULL) {
			fprintf(stderr, "ERROR: couldn't parse line:\n%s\n",line);
			continue;
		}
		flash.front = lineItems[0];
		flash.back = lineItems[1];
		flash.repetitions = strtoumax(lineItems[2], NULL, 10);
		free(lineItems[2]);
		flash.easinessFactor = strtof(lineItems[3], NULL);
		free(lineItems[3]);
		flash.dueDay = strtoumax(lineItems[4], NULL, 10);
		free(lineItems[4]);
		flashcards[loadedFlashcards] = flash;
		loadedFlashcards++;
	}
	if(loadedFlashcards == 0) {
			fprintf(stderr,"ERROR: file is empty\n");
			exit(EXIT_FAILURE);
	}
	free(line);
	return loadedFlashcards;
}
int
main(int argc, char *argv[])
{
	char *import_file, *export_file, *review_file;
	int opt;
	while ((opt = getopt(argc, argv, "i:o:d:")) != -1) {
		switch (opt) {
		case 'i':
			import_file = optarg;
			break;
		case 'o':
			export_file = optarg;
			break;
		case 'd':
			review_file = optarg;
			break;
		default:
			fprintf(stderr, "Usage %s [-i input.csv] [-o output.csv] [-d] review.gdbm\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if(argc-optind == 1)
		review_file = argv[optind];

	if(!review_file) {
		fprintf(stderr, "Usage %s [-i input.csv] [-o output.csv] -d review.gdbm\n",argv[0]);
		exit(EXIT_FAILURE);
	}


	FILE *cards;
	time_t start_time, end_time;
	int score_histogram[] = {0,0,0,0,0,0};
	cards = fopen(review_file, "r");
	if (cards == NULL) {
		fprintf(stderr,"ERROR: Could not open file %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}

	struct flashcard flashcards[FLASHCARDS_TO_LOAD]; //flashcards in memory

	printf("Loading flashcards!\n");
	struct timespec start_loading,finished_loading;
	clock_gettime(CLOCK_REALTIME,&start_loading);
	int loadedFlashcards = csv_to_flashcard(cards, flashcards, FLASHCARDS_TO_LOAD);
	if(loadedFlashcards < 1) {
			fprintf(stderr,"ERROR: No flashcards loaded\n");
			exit(EXIT_FAILURE);
	}
	fclose(cards);
	clock_gettime(CLOCK_REALTIME,&finished_loading);
	printf("Flashcards loaded in %ld seconds and %ld nanoseconds\n",finished_loading.tv_sec-start_loading.tv_sec,finished_loading.tv_nsec-start_loading.tv_nsec);

	start_time = time(NULL);

	srand(time(NULL));
	char c = '\n';
	int throwaway;
	int repeat = 0;
	int today = ((unsigned long)time(NULL))/86400; //unix time in days
	int increment;
	int i = rand()%loadedFlashcards;
	do {

		do {
			increment = rand() % loadedFlashcards;
		} while(gcd(loadedFlashcards, increment) != 1);

		repeat = 0;
		for(int k = 0; k < loadedFlashcards; k++) {
			i += increment;
			if(i >= loadedFlashcards)
				i -= loadedFlashcards;
			if(flashcards[i].dueDay <= today) {
				printf("%s\n",flashcards[i].front);
				printf("Your guess: ");
				while((throwaway = getchar()) != '\n' && throwaway != EOF);
				if(throwaway == EOF) {
					end_time = time(NULL);
					print_user_stats(start_time,end_time,score_histogram);
					save_and_quit(argv[1],flashcards,loadedFlashcards);
				}
				printf("The answer was: %s\n",flashcards[i].back);
				do {
					printf("How did you do (0-5): ");
					fflush(stdout);
					do {
					c = getchar();
					} while(c == ' ' || c == '\t'); //get rid of leading whitespace
					if(c != '\n' && c != (char) EOF) while((throwaway = getchar()) != '\n' && throwaway != EOF); //clean up stdin
					if(c == (char) EOF || throwaway == EOF) {
						end_time = time(NULL);
						print_user_stats(start_time, end_time, score_histogram);
						save_and_quit(argv[1],flashcards,loadedFlashcards);
					}
				} while(c < '0' || c > '5');
				score_histogram[c-'0']++;
				flashcards[i].dueDay = today + repetitionInterval(flashcards[i].easinessFactor, flashcards[i].repetitions, c - '0');
				if(c < '3') {
					flashcards[i].repetitions = 0;
					repeat = 1;
				} else {
					flashcards[i].repetitions += 1;
				}
				flashcards[i].easinessFactor = newEasinessFactor(flashcards[i].easinessFactor,c-'0');
				if(flashcards[i].dueDay == today)
					printf("You'll see me again today!\n");
				else if(flashcards[i].dueDay == today + 1)
					printf("You'll see me again tomorrow!\n");
				else
					printf("You'll see me in %d days!\n",flashcards[i].dueDay - today);
				printf("\n");
			}
		}
	} while(repeat);

	end_time = time(NULL);
	print_user_stats(start_time, end_time, score_histogram);
	save_and_quit(argv[1],flashcards,loadedFlashcards);
}

