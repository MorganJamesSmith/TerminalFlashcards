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

If you type the null character (usually Control+D) then is will save your progress and quit.

## Future Features:
- More detail on the history of flashcards so you can graph your progress
- Possibly some kind of image viewer (either in terminal like ranger or just
open an external program for better compatibility)

## Coding Style
[Suckless Coding Style](https://suckless.org/coding_style/)

