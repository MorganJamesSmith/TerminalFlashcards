/* Flashcard spaced repetitions algorithim (SM2)
Score:
    5 - perfect response
    4 - correct response after a hesitation
    3 - correct response recalled with serious difficulty
    2 - incorrect response; where the correct one seemed easy to recall
    1 - incorrect response; the correct one remembered
    0 - complete blackout.
*/

int repetitionInterval(float easinessFactor, int repetitions, int score){
    if(easinessFactor == 0) easinessFactor = 2.5;
    if(score < 3) return 0;
    int interval = 1;
    if(repetitions == 2) interval = 6;
    else if(repetitions > 2) interval = repetitionInterval(easinessFactor, repetitions-1,score) * easinessFactor;
    return interval;
}

float newEasinessFactor(float oldEasinessFactor, int score){
    if(oldEasinessFactor == 0) return 2.5;
    float easinessFactor = (oldEasinessFactor)+(0.1-(5-score)*(0.08+(5-score)*0.02));
    if(easinessFactor < 1.3)
        easinessFactor = 1.3;
    return easinessFactor;
}
