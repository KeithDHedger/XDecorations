
PROGRAM = xdecorations
SRC =  $(PROGRAM).c toon_root.c
LIBS = -L/usr/lib64 -lXext -lX11 -lm -lXpm
DATADIR = "`/bin/pwd`/pixmaps"

all:
	gcc -o $(PROGRAM)  -DDATADIR=\"${DATADIR}\" $(SRC) $(LIBS)

clean:
	rm $(PROGRAM)
