
SUBDIRS = XDecorations/app

remake:
	touch ./remakesourcelist
	$(shell ./remakesourcelist)

install-exec-hook:
	mkdir -vp $(DESTDIR)${pkgdatadir}
	cp -r XDecorations/resources/pixmaps $(DESTDIR)${pkgdatadir}
