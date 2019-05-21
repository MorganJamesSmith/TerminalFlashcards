CC = tcc

CFLAGS = -g -Wunsupported -Wwrite-strings -Werror -Wall

main : main.c csvparser.c

clean :
	rm main
