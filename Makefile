
PROGRAM = xdecorations
SRC =  $(PROGRAM).c toon_root.c
LIBS = -L/usr/lib64 -lXext -lX11 -lm -lXpm
PREFIX = /usr
DESTDIR = ${DESTDIR:-""}
DATADIR = "`/bin/pwd`/pixmaps"

all:
	gcc -o $(PROGRAM)  -DDATADIR=\"${DATADIR}\" $(SRC) $(LIBS)

install:
	gcc -o $(PROGRAM)  -DDATADIR=\"${DATADIR}\" $(SRC) $(LIBS)
	mkdir -vp $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps $(DESTDIR)$(PREFIX)/bin||true
	cp -r pixmaps/* $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps||true
	cp $(PROGRAM) $(DESTDIR)$(PREFIX)/bin||true
	
clean:
	rm $(PROGRAM)
