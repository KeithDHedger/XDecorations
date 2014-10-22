#if 0
©Keith Hedger Mon 20 Oct 14:21:11 BST 2014 kdhedger68713@gmail.com
*   This program is free software;
you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation;
either version 2 of the License,or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY;
without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should hargve received a copy of the GNU General Public License
*   along with this program;
if not,write to the Free Software
*   Foundation,Inc.,59 Temple Place,Suite 330,Boston,MA  02111-1307  USA

*   toon_root.c Copyright (C) 1999-2001  Robin Hogan
#endif

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/xpm.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "toon.h"

#define MAXPATHNAMELEN 2048
#define NUMBEROFFLYERS 2

char		pathname[MAXPATHNAMELEN];

Display*	display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
int			centerX;
int			centerY;
GC			gc;

int			done=0;
long		mainDelay=50000;

Pixmap		figurePixmap[2];
Pixmap		figureMaskPixmap[2];

Pixmap		lampsPixmap[2];
Pixmap		lampsMaskPixmap[2];

Pixmap		treePixmap[2];
Pixmap		treeMaskPixmap[2];

Pixmap		starPixmap[2];
Pixmap		starMaskPixmap[2];

Pixmap		treeLampsPixmap[8];
Pixmap		treeLampsMaskPixmap[8];

Pixmap		tinselPixmap;
Pixmap		tinselMaskPixmap;

Pixmap		flyersPixmap[NUMBEROFFLYERS];
Pixmap		flyersMaskPixmap[NUMBEROFFLYERS];

int			onOff=0;

uint		runCounter=0;

int			flyersSpeed=1;
int			flyersStep=10;
int			flyersWidth[NUMBEROFFLYERS];
int			flyersHeight[NUMBEROFFLYERS];
int			flyersX[NUMBEROFFLYERS];
int			flyersY[NUMBEROFFLYERS];
bool		showFlyers=false;
int			flyersMaxY=400;
int			flyersActive[NUMBEROFFLYERS];
int			flyerSpread=400;

int			figureSpeed=10;
int			figureX=100;
int			figureY=100;
int			figureW;
int			figureH;
int			figureCount=0;
bool		showFigure=false;
int			figureOnOff=0;

int			lampSpeed=10;
int			lampX=0;
int			lampY=26;
int			lampWidth;
int			lampHeight;
int			lampCount=0;
bool		showLamps=false;
int			lampOffset=0;

int			treeWidth;
int			treeHeight;
int			treeNumber=1;
int			treelampSpeed=8;
int			starSpeed=6;
int			treeX=100;
int			treeY=100;
int			treeLampSet=1;
bool		showTinsel=false;
bool		showStar=false;
bool		showTree=false;
bool		showTreeLamps=false;

int			treeOnOff=0;
int			starOnOff=0;

char*		prefix;

bool		treeNeedsUpdate=false;
bool		flyerNeedsUpdate=false;
bool		figureNeedsUpdate=false;

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

args	xdecorations_rc[]=
{
//bools
	{"showflyer",TYPEBOOL,&showFlyers},
	{"showtree",TYPEBOOL,&showTree},
	{"showtinsel",TYPEBOOL,&showTinsel},
	{"showfigure",TYPEBOOL,&showFigure},
	{"showlamps",TYPEBOOL,&showLamps},
	{"showstar",TYPEBOOL,&showStar},
	{"showtreelamps",TYPEBOOL,&showTreeLamps},
//strings
	{"holiday",TYPESTRING,&prefix},
//ints
	{"speed",TYPEINT,&mainDelay},
	{"flyermaxy",TYPEINT,&flyersMaxY},
	{"spread",TYPEINT,&flyerSpread},
	{"lampspeed",TYPEINT,&lampSpeed},
	{"flyspeed",TYPEINT,&flyersSpeed},
	{"flystep",TYPEINT,&flyersStep},
	{"lampy",TYPEINT,&lampY},
	{"treelampspeed",TYPEINT,&treelampSpeed},
	{"treelampset",TYPEINT,&treeLampSet},
	{"treenumber",TYPEINT,&treeNumber},
	{"treex",TYPEINT,&treeX},
	{"treey",TYPEINT,&treeY},
	{"starspeed",TYPEINT,&starSpeed},
	{"figurex",TYPEINT,&figureX},
	{"figurey",TYPEINT,&figureY},
	{"figurespeed",TYPEINT,&figureSpeed},
	{NULL,0,NULL}
};

void loadVarsFromFile(char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%as %as",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(strarg);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													free(*(char**)(dataptr[cnt].data));
												sscanf(buffer,"%*s %a[^\n]s",(char**)dataptr[cnt].data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
												break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
					argname=NULL;
					strarg=NULL;
				}
			fclose(fd);
		}
}

void signalHandler()
{
	done=1;
}

int randInt(int maxVal)
{
	return rand() % maxVal;
}

void uSsleep(unsigned long usec)
{
	struct timeval t;
	t.tv_usec=usec%(unsigned long)1000000;
	t.tv_sec=usec/(unsigned long)1000000;
	select(0,(fd_set* )0,(fd_set* )0,(fd_set* )0,&t);
}

void initFlyers(void)
{
	int				rc=0;
	int				j=0;
	XpmAttributes	attrib;

	attrib.valuemask=0;

	rc=0;
	for(j=0; j<NUMBEROFFLYERS;j++)
		{
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sFly%i.xpm",DATADIR,prefix,j+1);
			rc+=XpmReadFileToPixmap(display,rootWin,pathname,&flyersPixmap[j],&flyersMaskPixmap[j],&attrib);
			flyersWidth[j]=attrib.width;
			flyersHeight[j]=attrib.height;
			flyersY[j]=(rand() % flyersMaxY);
			flyersX[j]=10;
			if((rand() % flyerSpread)==0)
				flyersActive[j]=1;
			else
				flyersActive[j]=0;
				
		}
	if(rc!=0)
		showFlyers=false;
}

void initFigure(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;

	asprintf(&lampseton,"%s/%sFigureOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sFigureOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&figurePixmap[0],&figureMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&figurePixmap[1],&figureMaskPixmap[1],&attrib);
	if(rc!=0)
		showFigure=false;
	figureW=attrib.width;
	figureH=attrib.height;

	free(lampseton);
	free(lampsetoff);
}

void initLamps(void)
{
	int				rc=0;
	XpmAttributes	attrib;
	char*			lampseton;
	char*			lampsetoff;

	attrib.valuemask=0;

	asprintf(&lampseton,"%s/%sLampsOn.xpm",DATADIR,prefix);
	asprintf(&lampsetoff,"%s/%sLampsOff.xpm",DATADIR,prefix);

	rc+=XpmReadFileToPixmap(display,rootWin,lampsetoff,&lampsPixmap[0],&lampsMaskPixmap[0],&attrib);
	rc+=XpmReadFileToPixmap(display,rootWin,lampseton,&lampsPixmap[1],&lampsMaskPixmap[1],&attrib);
	if(rc!=0)
		showLamps=false;
	lampWidth=attrib.width;
	lampHeight=attrib.height;

	lampCount=(displayWidth/lampWidth)+1;
	lampOffset=(displayWidth-(lampWidth*lampCount))/2;

	free(lampseton);
	free(lampsetoff);
}

void initTree(void)
{
	int				rc=0;
	int				j=0;
	XpmAttributes	attrib;

	attrib.valuemask=0;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sTree1.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&treePixmap[0],&treeMaskPixmap[0],&attrib);
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sTree2.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&treePixmap[1],&treeMaskPixmap[1],&attrib);
	if(rc!=0)
		showTree=false;

	treeWidth=attrib.width;
	treeHeight=attrib.height;

	rc=0;
	for(j=0; j<8; j++)
		{
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sTreeLights%i.xpm",DATADIR,prefix,j+1);
			rc+=XpmReadFileToPixmap(display,rootWin,pathname,&treeLampsPixmap[j],&treeLampsMaskPixmap[j],&attrib);

		}
	if(rc!=0)
		showTreeLamps=false;

	rc=0;
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar0.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&starPixmap[0],&starMaskPixmap[0],&attrib);
	snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar1.xpm",DATADIR,prefix);
	rc+=XpmReadFileToPixmap(display,rootWin,pathname,&starPixmap[1],&starMaskPixmap[1],&attrib);
	if(rc!=0)
		showStar=false;

	rc=0;
	rc+=XpmReadFileToPixmap(display,rootWin,DATADIR "/Tinsel.xpm",&tinselPixmap,&tinselMaskPixmap,&attrib);
	if(rc!=0)
		showTinsel=false;
}

void drawFlyers(void)
{
	int rc;
	int	j;

	Region	flyreg=XCreateRegion();
	Region	final;
	XRectangle AddRect;

	if (showFlyers==true)
		{
			for(j=0;j<NUMBEROFFLYERS;j++)
				{
					if(flyersActive[j]==1)
						{
							rc=XSetClipMask(display,gc,flyersMaskPixmap[j]);
							rc=XSetClipOrigin(display,gc,flyersX[j],flyersY[j]);
							rc=XCopyArea(display,flyersPixmap[j],rootWin,gc,0,0,flyersWidth[j],flyersHeight[j],flyersX[j],flyersY[j]);
						}
				}
		}
}

void drawFigure(void)
{
	int rc;
	if (showFigure==true)
		{
			rc=XSetClipMask(display,gc,figureMaskPixmap[figureOnOff]);
			rc=XSetClipOrigin(display,gc,figureX,figureY);
			rc=XCopyArea(display,figurePixmap[figureOnOff],rootWin,gc,0,0,figureW,figureH,figureX,figureY);
		}
}

void drawLamps(void)
{
	int rc;
	int loop;
	int	CurrentlampX=lampX;
	rc=XSetClipMask(display,gc,lampsMaskPixmap[onOff]);

	for (loop=0; loop<lampCount; loop++)
		{
			rc=XSetClipOrigin(display,gc,CurrentlampX+lampOffset,lampY);
			rc=XCopyArea(display,lampsPixmap[onOff],rootWin,gc,0,0,lampWidth,lampHeight,CurrentlampX+lampOffset,lampY);
			CurrentlampX+=lampWidth;
		}
}

void updateLamps(void)
{
	onOff=(onOff+1) & 1;
}

void drawTreeLamps(void)
{
	int rc;
	int lampset;

	lampset=(treeLampSet-1)*4;
	rc=XSetClipMask(display,gc,treeMaskPixmap[treeNumber-1]);
	rc=XSetClipOrigin(display,gc,treeX,treeY);
	rc=XCopyArea(display,treePixmap[treeNumber-1],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if(showTreeLamps==true)
		{
			rc=XSetClipMask(display,gc,treeLampsMaskPixmap[treeOnOff+lampset]);
			rc=XCopyArea(display,treeLampsPixmap[treeOnOff+lampset],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if (showStar==true)
		{
			rc=XSetClipMask(display,gc,starMaskPixmap[starOnOff]);
			rc=XCopyArea(display,starPixmap[starOnOff],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if (showTinsel==true)
		{
			rc=XSetClipMask(display,gc,tinselMaskPixmap);
			rc=XCopyArea(display,tinselPixmap,rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}
}

void updateTreeLamps(void)
{
	treeOnOff++;

	if (treeOnOff==4)
		treeOnOff=0;
}

void updateStar(void)
{
	starOnOff=(starOnOff+1) & 1;
}

void updateFigure(void)
{
	figureOnOff=(figureOnOff+1) & 1;
}

void updateFlyers(void)
{
	int	j=0;
int rc;
	for(j=0;j<NUMBEROFFLYERS;j++)
		{
			if((flyersActive[j]==0) && ((rand() % flyerSpread)==0))
				flyersActive[j]=1;

			if(flyersActive[j]==1)
				{
rc=XClearArea(display,rootWin,flyersX[j],flyersY[j],flyersWidth[j],flyersHeight[j],False);
					flyersX[j]+=flyersStep;

					if(flyersX[j]>displayWidth)
						{
							flyersActive[j]=0;
							flyersX[j]=0;
							flyersY[j]=(rand() % flyersMaxY);
						}
				}
		}
}

void eraseRects(void)
{
	int	rc=0;
	int	j;
//
//	if (showTree==true)
//		rc=XClearArea(display,rootWin,treeX,treeY,treeWidth,treeHeight,False);
//
//
//	if (showFigure==true)
//		rc=XClearArea(display,rootWin,figureX,figureY,figureW,figureH,False);
//	if (showFlyers==true)
//		{
//			for(j=0;j<NUMBEROFFLYERS;j++)
//				rc=XClearArea(display,rootWin,flyersX[j],flyersY[j],flyersWidth[j],flyersHeight[j],False);
//printf("%in",rc);
//		}

//	treeNeedsUpdate=true;
//	figureNeedsUpdate=true;
//	flyerNeedsUpdate=true;

	if ((showTree==true) && (treeNeedsUpdate==true))
		rc=XClearArea(display,rootWin,treeX,treeY,treeWidth,treeHeight,False);

	if ((showFigure==true) && (figureNeedsUpdate==true))
		rc=XClearArea(display,rootWin,figureX,figureY,figureW,figureH,False);

	if ((showFlyers==true) && (flyerNeedsUpdate==true))
		{
			for(j=0;j<NUMBEROFFLYERS;j++)
				rc=XClearArea(display,rootWin,flyersX[j],flyersY[j],flyersWidth[j],flyersHeight[j],False);
		}

	treeNeedsUpdate=false;
	figureNeedsUpdate=false;
	flyerNeedsUpdate=false;
}

int main(int argc,char* argv[])
{
	int argnum;
	char* arg;
	XEvent ev;
	Window root;
	int screen;
	Window		parentWindow;

	prefix=strdup("Xmas");

	asprintf(&arg,"%s/.config/xdecorations.rc",getenv("HOME"));
	loadVarsFromFile(arg,xdecorations_rc);
	free(arg);
//command line options.
	for (argnum=1; argnum<argc; argnum++)
		{
			arg=argv[argnum];


			if (strcmp(arg,"-showflyer")==0)
				showFlyers=true;

			if (strcmp(arg,"-showtree")==0)
				showTree=true;

			if (strcmp(arg,"-showlamps")==0)
				showLamps=true;

			if (strcmp(arg,"-showfigure")==0)
				showFigure=true;

			if (strcmp(arg,"-showstar")==0)
				showStar=true;

			if (strcmp(arg,"-showtinsel")==0)
				showTinsel=true;

			if (strcmp(arg,"-showtreelamps")==0)
				showTreeLamps=true;

			if (strcmp(arg,"-holiday")==0)
				prefix=argv[++argnum];

			if (strcmp(arg,"-speed")==0)
				mainDelay=atol(argv[++argnum]);
	
			if (strcmp(arg,"-flyermaxy")==0)
				flyersMaxY=atol(argv[++argnum]);

			if (strcmp(arg,"-spread")==0)
				flyerSpread=atol(argv[++argnum]);

			if (strcmp(arg,"-lampspeed")==0)
				lampSpeed=atol(argv[++argnum]);

			if (strcmp(arg,"-flyspeed")==0)
				flyersSpeed=atol(argv[++argnum]);

			if (strcmp(arg,"-flystep")==0)
				flyersStep=atol(argv[++argnum]);

			if (strcmp(arg,"-lampy")==0)
				lampY=atol(argv[++argnum]);

			if (strcmp(arg,"-treelampspeed")==0)
				treelampSpeed=atol(argv[++argnum]);

			if (strcmp(arg,"-treelampset")==0)
				treeLampSet=atol(argv[++argnum]);

			if (strcmp(arg,"-treenumber")==0)
				treeNumber=atol(argv[++argnum]);

			if (strcmp(arg,"-treex")==0)
				treeX=atol(argv[++argnum]);

			if (strcmp(arg,"-treey")==0)
				treeY=atol(argv[++argnum]);

			if (strcmp(arg,"-starspeed")==0)
				starSpeed=atol(argv[++argnum]);

			if (strcmp(arg,"-figurex")==0)
				figureX=atol(argv[++argnum]);

			if (strcmp(arg,"-figurey")==0)
				figureY=atol(argv[++argnum]);

			if (strcmp(arg,"-figurespeed")==0)
				figureSpeed=atol(argv[++argnum]);
		}

	srand((int)time((long* )NULL));

	signal(SIGKILL,(sighandler_t)&signalHandler);
	signal(SIGTERM,(sighandler_t)signalHandler);
	signal(SIGQUIT,(sighandler_t)signalHandler);
	signal(SIGINT,(sighandler_t)signalHandler);
	signal(SIGHUP,(sighandler_t)signalHandler);

	display=XOpenDisplay(NULL);
	if (display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	rootWin=ToonGetRootWindow(display,screen,&parentWindow);

	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	centerX=displayWidth / 2;
	centerY=displayHeight / 2;

	initLamps();
	initTree();
	initFigure();
	initFlyers();

	gc=XCreateGC(display,rootWin,0,NULL);
	XSetFillStyle(display,gc,FillStippled);

	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);

	while (!done)
		{
			while (XPending(display))
				{
					XNextEvent(display,&ev);

				}
			uSsleep(mainDelay);

			runCounter++;

	//		eraseRects();

			if (showLamps==true)
				{
					if ((runCounter % lampSpeed)==0)
						updateLamps();
					drawLamps();
				}

			if (showTree==true)
				{
					if ((runCounter % treelampSpeed)==0)
						{
							updateTreeLamps();
							treeNeedsUpdate=true;
						}

					if ((runCounter % starSpeed)==0 && showStar==true)
						updateStar();

					//drawTreeLamps();
				}

			if (showFigure==true)
				{
					if ((runCounter % figureSpeed)==0)
						{
							updateFigure();
							figureNeedsUpdate=true;
						}
					//drawFigure();
				}

			if (showFlyers==true)
				{
					if ((runCounter % flyersSpeed)==0)
						{
							updateFlyers();
							flyerNeedsUpdate=true;
						}
				//	drawFlyers();
				}
			eraseRects();
								drawTreeLamps();
				drawFigure();
					drawFlyers();


		}
	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	exit(0);
}


