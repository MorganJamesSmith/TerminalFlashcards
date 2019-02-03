main : main.o csvparser.o flashcard.o csvparser.h flashcard.h
	gcc -o main main.o csvparser.o flashcard.o
main.o : csvparser.h flashcard.h
	gcc -g -c main.c
flashcard.o : flashcard.c flashcard.h
	gcc -g -c flashcard.c
csvparser.o : csvparser.c csvparser.h
	gcc -g -c csvparser.c
clean :
	rm main main.o csvparser.o flashcard.o
