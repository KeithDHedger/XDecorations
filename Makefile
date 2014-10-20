
PROGRAM = xdecorations
SRC =  $(PROGRAM).c toon_root.c
LIBS = -L/usr/lib64 -lXext -lX11 -lm -lXpm

all:
	gcc -o $(PROGRAM) $(SRC) $(LIBS)

clean:
	rm $(PROGRAM)
