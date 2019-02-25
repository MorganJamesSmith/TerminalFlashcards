CC = gcc
CFLAGS = -funsigned-char -g
CFLAGS += -fno-strict-aliasing -ffunction-sections -fdata-sections
CFLAGS += --param max-inline-insns-single=500
CFLAGS += -gstrict-dwarf

CFLAGS += -Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith
CFLAGS += -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type
CFLAGS += -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast
CFLAGS += -Wwrite-strings -Wsign-compare -Waggregate-return -Wmissing-declarations -Wformat -Wmissing-format-attribute
CFLAGS += -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wcast-align


main : main.o csvparser.o flashcard.o csvparser.h flashcard.h

main.o : csvparser.h flashcard.h main.c

flashcard.o : flashcard.c flashcard.h

csvparser.o : csvparser.c csvparser.h

clean :
	rm main main.o csvparser.o flashcard.o
