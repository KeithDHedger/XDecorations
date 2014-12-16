APPNAME = XDecorations
PROGRAM = xdecorations
SRC =  $(PROGRAM).cpp toon_root.cpp
LIBS = -L/usr/lib64 -lXext -lX11 -lm -lXpm -lImlib2
PREFIX = /usr
DESTDIR = ${DESTDIR:-""}
LOCALDATADIR = "`/bin/pwd`/pixmaps"
INSTALLDATADIR = $(PREFIX)/share/$(PROGRAM)/pixmaps

all:
	g++ -o $(PROGRAM) -DDATADIR=\"${LOCALDATADIR}\" -Wall $(SRC) $(LIBS)

install:
	g++ -o $(PROGRAM) -DDATADIR=\"${INSTALLDATADIR}\" $(SRC) $(LIBS)
	mkdir -vp $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/share/doc/$(APPNAME) ||true
	cp -r pixmaps/* $(DESTDIR)$(PREFIX)/share/$(PROGRAM)/pixmaps||true
	cp $(PROGRAM) $(DESTDIR)$(PREFIX)/bin||true
	cp xdecorations.rc $(DESTDIR)$(PREFIX)/share/doc/$(APPNAME) ||true
	

uninstall:
	rm -r $(DESTDIR)$(PREFIX)/bin/$(PROGRAM) $(DESTDIR)$(PREFIX)/share/$(PROGRAM)

clean:
	rm $(PROGRAM)||true

installconfig:
	cp $(PROGRAM).rc ~/.config

makeball:
	gcc -o ball double.c  -L/usr/lib64 -lXext -lX11 -lm -lXpm