# Terminal Spaced Repetition App

## How to run:

Only tested on my Linux. Your milage may vary.

```
make
./main cards.csv
```

Use make to create the main executable. Then run it, passing the path of a csv file as an argument.
An example csv can be cound in the examples folder.

## Features

This app uses the SM-2 spaced repetition algorithim to show you flashcards just
as you might forgot the contents. This allows you to learn the facts on the
flashcards with the fewest repetitions.

It accepts a csv file as input and modifies it when run. The input csv file is
first backed up to a backup folder with the current unix timestamp before
changes are made.

## Future Features:
- Terminating the program midway through will save your progress
- More detail on the history of flashcards so you can graph your progress
- More rubust CSV parsing (fewer seg faults)
- Possibly some kind of image viewer (either in terminal like ranger or just
open an external program for better compatibility)

## Code layout

### main

- check command line argument
- open csv file
- parse it using csvparser
- start flashcard printing loop
- print flashcard front
- throw away user input
- show flashcard back
- keep asking for score until proper input is given
- update flashcard easinessfactor and duedate
- tell user next duedate
- continue for rest of flashcards
- copy csv file to backup location
- print all flashcards in original csv location
- end

### flashcard

#### Flashcard Struct

```
struct flashcard {
    char *front;
    char *back;
    int repetitions;
    float easinessFactor;
    int dueDay;
    int lineNumber;
};
```
#### Functions:

Returns the number of days until you will see a flashcard again depending on its easiness factor, the number of times you've done it, and how you scored it.

```int repetitionInterval(float easinessFactor, int repetitions, int score)```

Returns a new flashcard easiness factor based on the previous one and how you score it

```float newEasinessFactor(float oldEasinessFactor, int score)```

### csvparser
Converts CSV files to flashcard items.

#### Functions:

##### External Functions:
Returns a list of flashcard structs when passed a CSV file

```int csv_to_flashcard(FILE *csv, struct flashcard* flashcards[], int flashcardsToLoad)```

##### Internal Functions:

Returns a list of values when passed a line from a CSV

```char **parse_csv(const char *line)```

Returns the number of values when passed a line from CSV

```static int count_fields(const char *line)```
