#ifndef FLASHCARD_H
#define FLASHCARD_H

struct flashcard {
    char *front;
    char *back;
    int repetitions;
    float easinessFactor;
    int dueDay;
    int lineNumber;
};

int repetitionInterval(float easinessFactor, int repetitions, int score);
float newEasinessFactor(float oldEasinessFactor, int score);

#endif
