PROGRAM = xdecorations
SRC =  $(PROGRAM).cpp toon_root.cpp
LIBS = -L/usr/lib64 -lXext -lX11 -lm -lXpm
PREFIX = /usr
DESTDIR = ${DESTDIR:-""}
LOCALDATADIR = "`/bin/pwd`/pixmaps"
INSTALLDATADIR = $(PREFIX)/share/$(PROGRAM)/pixmaps

all:
	g++ -o $(PROGRAM) -DDATADIR=\"${LOCALDATADIR}\" $(SRC) $(LIBS)

install:
	g++ -o $(PROGRAM) -DDATADIR=\"${INSTALLDATADIR}\" $(SRC) $(LIBS)
	mkdir -vp $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps $(DESTDIR)$(PREFIX)/bin||true
	cp -r pixmaps/* $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps||true
	cp $(PROGRAM) $(DESTDIR)$(PREFIX)/bin||true

uninstall:
	rm -r $(DESTDIR)$(PREFIX)/bin/$(PROGRAM) $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps

clean:
	rm $(PROGRAM)||true

installconfig:
	cp $(PROGRAM).rc ~/.config