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
#include <X11/xpm.h>
#include <Imlib2.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "toon.h"

#define MAXPATHNAMELEN 2048
#define MAXNUMBEROFFLYERS 10
#define MAXNUMBEROFTREELIGHTS 10
#define MAXFLOAT 10
#define MAXFALLINGOBJECTS 100

#define VERSION "0.1.3"
#define _SELECTPIXMAP(a,b) (a+(2*b))//a=ONPIXMAP b=xxxOnOff

enum {ONPIXMAP=0,ONMASK,OFFPIXMAP,OFFMASK};

char		pathname[MAXPATHNAMELEN];
char*		configFilePath;

Display*	display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
GC			gc;
Visual*		visual=NULL;
int			depth=0;
Imlib_Image	image;
int			screen;
Region		rg;

int			done=0;
long		mainDelay=20000;

uint		runCounter=0;
bool		useWindow=true;

struct		objects
{
	Pixmap	pixmap;
	Pixmap	mask;
	int		h;
	int		w;
};

struct		movement
{
	objects	*object;
	int		x;
	int		y;
	int		deltaX;
	int		deltaY;
	int		stepX;
	int		stepY;
	bool	use;
};
	
//falling
objects		floating[MAXFLOAT];
movement	moving[MAXFALLINGOBJECTS];
//Pixmap		fallingPixmap[MAXNUMBEROFFALLING][2];
//int			fallingWidth[MAXNUMBEROFFALLING];
//int			fallingHeight[MAXNUMBEROFFALLING];
//int			fallingX[MAXNUMBEROFFALLING];
//int			fallingY[MAXNUMBEROFFALLING];
//int			fallingSpeed=10;
//int			fallingDeltaX=2;
//int			fallingDeltaY=2;
int			fallingCount=0;
int			numberOfFalling=100;
bool		showFalling=true;
int			fallSpeed=1;
int			fallYSpeed=4;
int			fallXpeed=20;
int			fallingStep=1;
int			xSwirl=40;
int			ySwirl=40;
int			leafSpread=400;
int			wind=4;
int			blusterSpeed=20;

//flyers
Pixmap		flyersPixmap[MAXNUMBEROFFLYERS][2];
int			flyersSpeed=1;
int			flyersStep=8;
int			flyersWidth[MAXNUMBEROFFLYERS];
int			flyersHeight[MAXNUMBEROFFLYERS];
int			flyersX[MAXNUMBEROFFLYERS];
int			flyersY[MAXNUMBEROFFLYERS];
bool		showFlyers=false;
int			flyersMaxY=400;
int			flyersActive[MAXNUMBEROFFLYERS];
int			flyerSpread=500;
int			flyerCount=0;

//figure
Pixmap		figurePixmap[4];
int			figureSpeed=100;
int			figureX=100;
int			figureY=100;
int			figureW;
int			figureH;
int			figureOnOff=0;
int			figureNumber=1;

//lamps
Pixmap		lampsPixmap[4];
int			lampSpeed=100;
int			lampX=0;
int			lampY=0;
int			lampWidth;
int			lampHeight;
int			lampCount;
int			lampSet=1;
int			lampsOnOff=0;

//trees
Pixmap		treePixmap[2];
int			treeWidth;
int			treeHeight;
int			treeNumber=1;
int			treelampSpeed=100;
int			starSpeed=100;
int			treeX;
int			treeY;
int			treeLampSet=1;
int			treeOnOff=0;
bool		showTree=false;

//tinsel`
Pixmap		tinselPixmap[2];
bool		showTinsel=false;

//star
Pixmap		starPixmap[4];
bool		showStar=false;
int			starOnOff=0;

//tree lamps
Pixmap		treeLampsPixmap[MAXNUMBEROFTREELIGHTS][2];
bool		showTreeLamps=false;
int			treeLampCount=0;

char*		prefix;

bool		treeNeedsUpdate=false;
bool		flyerNeedsUpdate=false;
bool		figureNeedsUpdate=false;
bool		fallingNeedsUpdate=false;

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};


enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

args	xdecorations_rc[]=
{
//bools
	{"flyer",TYPEBOOL,&showFlyers},
	{"tree",TYPEBOOL,&showTree},
	{"tinsel",TYPEBOOL,&showTinsel},
	{"star",TYPEBOOL,&showStar},
	{"treelamps",TYPEBOOL,&showTreeLamps},
	{"usewindow",TYPEBOOL,&useWindow},
//strings
	{"holiday",TYPESTRING,&prefix},
//ints
	{"delay",TYPEINT,&mainDelay},
	{"flyermaxy",TYPEINT,&flyersMaxY},
	{"spread",TYPEINT,&flyerSpread},
	{"lampdelay",TYPEINT,&lampSpeed},
	{"lampset",TYPEINT,&lampSet},
	{"flydelay",TYPEINT,&flyersSpeed},
	{"flystep",TYPEINT,&flyersStep},
	{"lampy",TYPEINT,&lampY},
	{"treelampdelay",TYPEINT,&treelampSpeed},
	{"treelampset",TYPEINT,&treeLampSet},
	{"treenumber",TYPEINT,&treeNumber},
	{"treex",TYPEINT,&treeX},
	{"treey",TYPEINT,&treeY},
	{"stardelay",TYPEINT,&starSpeed},
	{"figurex",TYPEINT,&figureX},
	{"figurey",TYPEINT,&figureY},
	{"figuredelay",TYPEINT,&figureSpeed},
	{"figure",TYPEINT,&figureNumber},
	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
						case TYPEINT:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

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

bool randomEvent(int max)
{
	if((random() % max)==0)
		return(true);

	return(false);
}

int	randomDirection(void)
{
	int	r;

	if(randomEvent(2)==true)
		r=1;
	else
		r=-1;
	return(r);
}

void initFlyers(void)
{

	flyerCount=0;

	for(int j=0; j<MAXNUMBEROFFLYERS; j++)
		{
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sFly%i.png",DATADIR,prefix,j+1);
			image=imlib_load_image(pathname);
			if(image!=NULL)
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&flyersPixmap[flyerCount][ONPIXMAP],&flyersPixmap[flyerCount][ONMASK]);
					flyersWidth[flyerCount]=imlib_image_get_width();
					flyersHeight[flyerCount]=imlib_image_get_height();
					flyersY[flyerCount]=(rand() % flyersMaxY);
					flyersX[flyerCount]=0-flyersWidth[j];
					if((rand() % flyerSpread)==0)
						flyersActive[flyerCount]=1;
					else
						flyersActive[flyerCount]=0;
					flyerCount++;
				}
		}

	if(flyerCount==0)
		showFlyers=false;
}

void initFigure(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sFigureOn%i.png",DATADIR,prefix,figureNumber);

	image=imlib_load_image(pathname);
	if(image==NULL)
		figureNumber=0;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(rootWin);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&figurePixmap[ONPIXMAP],&figurePixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%sFigureOff%i.png",DATADIR,prefix,figureNumber);
			image=imlib_load_image(pathname);
			if(image==NULL)
				figureNumber=0;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&figurePixmap[OFFPIXMAP],&figurePixmap[OFFMASK]);

					figureW=imlib_image_get_width();
					figureH=imlib_image_get_height();
				}
		}
}

void initLamps(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sLampsOn%i.png",DATADIR,prefix,lampSet);
	image=imlib_load_image(pathname);
	if(image==NULL)
		lampSet=0;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(rootWin);
			imlib_image_set_has_alpha(1);
//imlib_render_image_on_drawable_at_size(0, 0, 200, 200);
			imlib_render_pixmaps_for_whole_image(&lampsPixmap[ONPIXMAP],&lampsPixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%sLampsOff%i.png",DATADIR,prefix,lampSet);
			image=imlib_load_image(pathname);
			if(image==NULL)
				lampSet=0;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
//imlib_render_image_on_drawable_at_size(0, 0, 200, 200);
					imlib_render_pixmaps_for_whole_image(&lampsPixmap[OFFPIXMAP],&lampsPixmap[OFFMASK]);

					lampWidth=imlib_image_get_width();
					lampHeight=imlib_image_get_height();
					lampCount=(displayWidth/lampWidth)+1;
				}
		}
}

void initFalling(void)
{
	int	floatnumber;

	fallingCount=0;

	for(int j=0;j<MAXFLOAT;j++)
		{
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sFloat%i.png",DATADIR,prefix,j+1);
			image=imlib_load_image(pathname);
			if(image!=NULL)
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&floating[fallingCount].pixmap,&floating[fallingCount].mask);
					floating[fallingCount].w=imlib_image_get_width();
					floating[fallingCount].h=imlib_image_get_height();
					fallingCount++;
				}
		}

	if(fallingCount==0)
		showFalling=false;
	else
		{
			for(int j=0;j<numberOfFalling;j++)
				{
					floatnumber=(rand() % fallingCount);
					moving[j].object=&floating[floatnumber];
					moving[j].x=(rand() % displayWidth);
					moving[j].y=0-moving[j].object->h;
					moving[j].deltaX=1;
					moving[j].deltaY=1;
					moving[j].stepX=1;
					moving[j].stepY=randInt(fallYSpeed)+1;
					moving[j].use=false;
				}
		}
}

void initTree(void)
{
	bool	gotsomelamps=false;

	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sTree%i.png",DATADIR,prefix,treeNumber);

	image=imlib_load_image(pathname);
	if(image==NULL)
		showTree=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(rootWin);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&treePixmap[ONPIXMAP],&treePixmap[ONMASK]);
			treeWidth=imlib_image_get_width();
			treeHeight=imlib_image_get_height();
		}

	treeLampCount=0;
	for(int j=0; j<MAXNUMBEROFTREELIGHTS; j++)
		{
			snprintf(pathname,MAXPATHNAMELEN,"%s/%sTreeLights%i.%i.png",DATADIR,prefix,treeLampSet,j+1);
			image=imlib_load_image(pathname);
			if(image!=NULL)
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&treeLampsPixmap[j][ONPIXMAP],&treeLampsPixmap[j][ONMASK]);
					treeLampCount++;
					gotsomelamps=true;
				}
		}
	if(gotsomelamps==false)
		showTreeLamps=false;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar0.png",DATADIR,prefix);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showStar=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(rootWin);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&starPixmap[ONPIXMAP],&starPixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%sStar1.png",DATADIR,prefix);
			image=imlib_load_image(pathname);
			if(image==NULL)
				showStar=false;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(rootWin);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&starPixmap[OFFPIXMAP],&starPixmap[OFFMASK]);
				}
		}

	snprintf(pathname,MAXPATHNAMELEN,"%s/%sTinsel.png",DATADIR,prefix);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showTinsel=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(rootWin);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&tinselPixmap[ONPIXMAP],&tinselPixmap[ONMASK]);
		}
}

void drawFlyers(void)
{
	int rc;
	int	j;

	if(showFlyers==false)
		return;

	for(j=0; j<flyerCount; j++)
		{
			if(flyersActive[j]==1)
				{
					rc=XSetClipMask(display,gc,flyersPixmap[j][ONMASK]);
					rc=XSetClipOrigin(display,gc,flyersX[j],flyersY[j]);
					rc=XCopyArea(display,flyersPixmap[j][ONPIXMAP],rootWin,gc,0,0,flyersWidth[j],flyersHeight[j],flyersX[j],flyersY[j]);
				}
		}
}

void drawFigure(void)
{
	int rc;

	if(figureNumber==0)
		return;

	rc=XSetClipMask(display,gc,figurePixmap[_SELECTPIXMAP(ONMASK,figureOnOff)]);
	rc=XSetClipOrigin(display,gc,figureX,figureY);
	rc=XCopyArea(display,figurePixmap[_SELECTPIXMAP(ONPIXMAP,figureOnOff)],rootWin,gc,0,0,figureW,figureH,figureX,figureY);
}

void drawLamps(void)
{
	int rc=0;
	int loop;
	int	CurrentlampX;

	if(lampSet==0)
		return;

	CurrentlampX=lampX;
	rc=XSetClipMask(display,gc,lampsPixmap[_SELECTPIXMAP(ONMASK,lampsOnOff)]);

	for (loop=0; loop<lampCount; loop++)
		{
			rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
			rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,lampsOnOff)],rootWin,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
			CurrentlampX+=lampWidth;
		}
}

void drawFalling(void)
{
	int rc;
	for(int j=0;j<numberOfFalling;j++)
		{
			rc=XSetClipMask(display,gc,moving[j].object->mask);
			rc=XSetClipOrigin(display,gc,moving[j].x,moving[j].y);
			rc=XCopyArea(display,moving[j].object->pixmap,rootWin,gc,0,0,moving[j].object->w,moving[j].object->h,moving[j].x,moving[j].y);
	}
}
#define MAXXSTEP 2             /* drift speed max */
#define WHIRLFACTOR 3
int SmoothWhirl = 1;
int WhirlAdapt;
int WhirlFactor = WHIRLFACTOR;
int	blusterDuration=3000;
int blusterEvent=2000;
int blusterCountdown=0;
int	blusterDirection=0;

void updateFalling(void)
{
blusterSpeed=40;
	for(int j=0;j<numberOfFalling;j++)
		{
			if(moving[j].use==true)
				{
					//if (SmoothWhirl)
					//	{
							WhirlAdapt=randInt(WhirlFactor); 
							//if(randInt(1000)<500)
								if(randomEvent(2)==true)
									WhirlAdapt=-WhirlAdapt;
							//''snow->xStep = snow->xStep + WhirlAdapt;
							moving[j].stepX=moving[j].stepX+WhirlAdapt;
							//if (snow->xStep > MaxXStep) snow->xStep = MaxXStep;
							if(moving[j].stepX>MAXXSTEP)
								moving[j].stepX=MAXXSTEP;
							if(moving[j].stepX<-MAXXSTEP)
								moving[j].stepX=-MAXXSTEP;
					//	}

//					if(randomEvent(ySwirl)==true)
//						{
//							
//						moving[j].stepY=(moving[j].stepY+(randomDirection()));
//						}
//						moving[j].deltaY=randomDirection();
//					else
//						moving[j].deltaY=1;
//
//					if(randomEvent(xSwirl)==true)
//						moving[j].deltaX=randomDirection();

					moving[j].y=moving[j].y+moving[j].stepY;
					//moving[j].x=moving[j].x+(moving[j].deltaX*moving[j].stepX);
					//moving[j].x=moving[j].x+(moving[j].stepX/fallXpeed);
					//moving[j].x=moving[j].x+moving[j].stepX+wind;
					
					if(blusterCountdown==0)
						{
							moving[j].x=moving[j].x+moving[j].stepX+wind;
							if(randomEvent(blusterEvent))
								{
									blusterCountdown=blusterDuration;
									blusterDirection=randomDirection();
								}
						}
					else
						{
							moving[j].x=moving[j].x+((blusterSpeed+randInt(blusterSpeed/10))*blusterDirection);
							blusterCountdown--;
							if(blusterCountdown<0)
								blusterCountdown=0;
						}			
					

					if(moving[j].y>displayHeight+moving[j].object->h)
						{
							moving[j].use=false;
							moving[j].y=0-moving[j].object->h;
						}
					
					if(moving[j].x>displayWidth+moving[j].object->w)
						moving[j].x=0-moving[j].object->w;
					
					if(moving[j].x<0-moving[j].object->w)
						moving[j].x=displayWidth;
				}
			else
				{
					if(randomEvent(leafSpread)==true)
						moving[j].use=true;
				}
		}
}

void updateLamps(void)
{
	lampsOnOff=(lampsOnOff+1) & 1;
}

void drawTreeLamps(void)
{
	int rc;
	int lampset;

	if(showTree==false)
		return;

	lampset=(treeLampSet-1)*4;
	rc=XSetClipMask(display,gc,treePixmap[ONMASK]);
	rc=XSetClipOrigin(display,gc,treeX,treeY);
	rc=XCopyArea(display,treePixmap[ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if(showTreeLamps==true)
		{
			rc=XSetClipMask(display,gc,treeLampsPixmap[treeOnOff][ONMASK]);
			rc=XCopyArea(display,treeLampsPixmap[treeOnOff][ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showStar==true)
		{
			rc=XSetClipMask(display,gc,starPixmap[_SELECTPIXMAP(ONMASK,starOnOff)]);
			rc=XCopyArea(display,starPixmap[_SELECTPIXMAP(ONPIXMAP,starOnOff)],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showTinsel==true)
		{
			rc=XSetClipMask(display,gc,tinselPixmap[ONMASK]);
			rc=XCopyArea(display,tinselPixmap[ONPIXMAP],rootWin,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}
}

void updateTreeLamps(void)
{
	treeOnOff++;

	if(treeOnOff==treeLampCount)
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

	for(j=0; j<flyerCount; j++)
		{
			if((flyersActive[j]==0) && ((rand() % flyerSpread)==0))
				flyersActive[j]=1;

			if(flyersActive[j]==1)
				{
					flyersX[j]+=flyersStep;

					if(flyersX[j]>displayWidth+flyersWidth[j])
						{
							flyersActive[j]=0;
							flyersX[j]=0-flyersWidth[j]*2;
							flyersY[j]=(rand() % flyersMaxY);
						}
				}
		}
}

void eraseRects(void)
{
	int	rc=0;
	int	j;

	if((figureNumber!=0) && (figureNeedsUpdate==true))
		{
			rc=XClearArea(display,rootWin,figureX,figureY,figureW,figureH,False);
			updateFigure();
		}

	if((showTree==true) && (treeNeedsUpdate==true))
		{
			rc=XClearArea(display,rootWin,treeX,treeY,treeWidth,treeHeight,False);
			updateTreeLamps();
		}

	if((showFlyers==true) && (flyerNeedsUpdate==true))
		{
			for(j=0; j<flyerCount; j++)
				rc=XClearArea(display,rootWin,flyersX[j],flyersY[j],flyersWidth[j],flyersHeight[j],False);
			updateFlyers();
		}

	if((showFalling==true) && (fallingNeedsUpdate==true))
		{
			for(int j=0;j<numberOfFalling;j++)
				{
					rc=XSetClipOrigin(display,gc,moving[j].x,moving[j].y);
					rc=XSetClipMask(display,gc,0);
					rc=XClearArea(display,rootWin,moving[j].x,moving[j].y,moving[j].object->w,moving[j].object->h,False);
				}
			updateFalling();
		}

	treeNeedsUpdate=false;
	figureNeedsUpdate=false;
	flyerNeedsUpdate=false;
	fallingNeedsUpdate=false;
}

void showUnShow(const char* arg1,const char* arg2,bool *value)
{
	const char*	ptr=NULL;

	ptr=strcasestr(arg1,arg2);
	if(ptr!=NULL)
		{
			if(strcasecmp(ptr,arg2)!=0)
				return;
			*value=true;
			if(strcasestr(arg1,"no-")!=NULL)
				*value=false;
		}
}

int get_argb_visual(Visual** vis, int *depth)
{
	/* code from gtk project, gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals = 0, i;
	visual_template.screen = screen;
	visual_list = XGetVisualInfo (display, 0,&visual_template, &nxvisuals);

	for (i = 0; i < nxvisuals; i++)
		{

			if (visual_list[i].depth == 32 &&
			        (visual_list[i].red_mask   == 0xff0000 &&
			         visual_list[i].green_mask == 0x00ff00 &&
			         visual_list[i].blue_mask  == 0x0000ff))
				{
					*vis = visual_list[i].visual;
					*depth = visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	XFree(visual_list);
	return 1;
}

void doHelp(void)
{
	printf("XDecorations (c)2014 K. D. Hedger - Version %s\n",VERSION);
	printf("Released under the gpl-3.0 license\n\n");
	printf("Values are set to defaults then set to values contained in ~/.config/xdecorations.rc and then overridden on the command line\n\n");

	printf("-holiday STRING\n");
	printf("\tSet prefix for theme\n");
	printf("-delay INTEGER\n");
	printf("\tSet main delay\n");
	printf("-configfile FILEPATH\n");
	printf("\tSet new config file ( using only one instance of this makes any sense )\n");
	printf("-writeconfig FILEPATH\n");
	printf("\tWrite out a new config file including currently loaded config file,defaults and command line options\n");
	printf("\tMust be last command on line\n\n");
	printf("-usewindow/-no-usewindow STRING\n");
	printf("\tUse a transparent window instead of the root window\n");

	printf("-showflyer/-no-showflyer\n");
	printf("\tShow flying objects\n");
	printf("-showtree/-no-showtree\n");
	printf("\tShow tree\n");
	printf("-showlamps/-no-showlamps\n");
	printf("\tShow lamps\n");
	printf("-showfigure/-no-showfigure\n");
	printf("\tShow figure\n");
	printf("-showstar/-no-showstar\n");
	printf("\tShow star\n");
	printf("-showtinsel/-no-showtinsel\n");
	printf("\tShow tinsel\n");
	printf("-showtreelamps/-no-showtreelamps\n");
	printf("\tShow tree lamps\n");
	printf("\n");
	printf("-lampy INTEGER\n");
	printf("\tLamp Y position\n");
	printf("-lampdelay INTEGER\n");
	printf("\tLamp delay\n");
	printf("-lampset INTEGER\n");
	printf("\tLamp set\n");
	printf("-flyermaxy INTEGER\n");
	printf("\tLowest point on screen for flying objects\n");
	printf("-spread INTEGER\n");
	printf("\tRandom delay for flying objects\n");
	printf("-flydelay INTEGER\n");
	printf("\tFlying objects delay\n");
	printf("-flystep INTEGER\n");
	printf("\tAmount to move flying objects\n");
	printf("-treelampdelay INTEGER\n");
	printf("\tTree lamps delay\n");
	printf("-treelampset INTEGER\n");
	printf("\tLampset to use on tree\n");
	printf("-treenumber INTEGER\n");
	printf("\tThe tree to use\n");
	printf("-treex INTEGER\n");
	printf("\tAbsolute X position of tree\n");
	printf("-treey INTEGER\n");
	printf("\tAbsolute Y position of tree\n");
	printf("-stardelay INTEGER\n");
	printf("\tDelay for star\n");
	printf("-figurex INTEGER\n");
	printf("\tAbsolute X position of figure\n");
	printf("-figuredelay INTEGER\n");
	printf("\tDelay for figure\n");
	printf("-figurenumber INTEGER\n");
	printf("\tNumber of figure to use\n\n");

	exit(0);
}

int main(int argc,char* argv[])
{
	int		argnum;
	const	char* argstr;
	XEvent	ev;
	Window	root;
	Window	parentWindow;
	int		rc=0;
	Hints	hints;
	Atom	xa;
	Atom	xa_prop[10];
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC,&now);
	srandom(now.tv_nsec);

//for(int j=0;j<10;j++)
//	printf("%in",randomDirection());
//exit(0);

	prefix=strdup("Xmas");
	asprintf(&configFilePath,"%s/.config/xdecorations.rc",getenv("HOME"));

	loadVarsFromFile(configFilePath,xdecorations_rc);
//command line options.
	for (argnum=1; argnum<argc; argnum++)
		{
			argstr=argv[argnum];

			showUnShow(argstr,"showflyer",&showFlyers);//showFlyers=false
			showUnShow(argstr,"showtree",&showTree);//showTree=false
			showUnShow(argstr,"showstar",&showStar);//showStar=false
			showUnShow(argstr,"showtinsel",&showTinsel);//showTinsel=false
			showUnShow(argstr,"showtreelamps",&showTreeLamps);//showTreeLamps=false
			showUnShow(argstr,"usewindow",&useWindow);//use/don't use window

			if(strcmp(argstr,"-configfile")==0)//~/.config/xdecorations.rc
				{
					free(configFilePath);
					asprintf(&configFilePath,"%s",argv[++argnum]);
					loadVarsFromFile(configFilePath,xdecorations_rc);
				}

			if(strcmp(argstr,"-holiday")==0)//Xmas
				{
					free(prefix);
					prefix=strdup(argv[++argnum]);
				}

			if(strcmp(argstr,"-delay")==0)//mainDelay=20000
				mainDelay=atol(argv[++argnum]);

			if(strcmp(argstr,"-lampy")==0)//lampY=16
				lampY=atol(argv[++argnum]);

			if(strcmp(argstr,"-lampdelay")==0)//lampSpeed=100
				lampSpeed=atol(argv[++argnum]);

			if(strcmp(argstr,"-lampset")==0)//lampSet=1
				lampSet=atol(argv[++argnum]);

			if(strcmp(argstr,"-flyermaxy")==0)//flyersMaxY=400
				flyersMaxY=atol(argv[++argnum]);

			if(strcmp(argstr,"-spread")==0)//flyerSpread=500
				flyerSpread=atol(argv[++argnum]);

			if(strcmp(argstr,"-flydelay")==0)//flyersSpeed=1
				flyersSpeed=atol(argv[++argnum]);

			if(strcmp(argstr,"-flystep")==0)//flyersStep=8
				flyersStep=atol(argv[++argnum]);

			if(strcmp(argstr,"-treelampdelay")==0)//treelampSpeed=100
				treelampSpeed=atol(argv[++argnum]);

			if(strcmp(argstr,"-treelampset")==0)//treeLampSet=1
				treeLampSet=atol(argv[++argnum]);

			if(strcmp(argstr,"-treenumber")==0)//treeNumber=1
				treeNumber=atol(argv[++argnum]);

			if(strcmp(argstr,"-treex")==0)//treeX=100
				treeX=atol(argv[++argnum]);

			if(strcmp(argstr,"-treey")==0)//treeY=100
				treeY=atol(argv[++argnum]);

			if(strcmp(argstr,"-stardelay")==0)//starSpeed
				starSpeed=atol(argv[++argnum]);

			if(strcmp(argstr,"-figurex")==0)//figureX=100
				figureX=atol(argv[++argnum]);

			if(strcmp(argstr,"-figure")==0)//figure=1
				figureNumber=atol(argv[++argnum]);

			if(strcmp(argstr,"-figurey")==0)//figureY=100
				figureY=atol(argv[++argnum]);

			if(strcmp(argstr,"-figuredelay")==0)//figureSpeed=100
				figureSpeed=atol(argv[++argnum]);

			if(strcmp(argstr,"-figurenumber")==0)//figureNumber=1
				figureNumber=atol(argv[++argnum]);

			if(strcmp(argstr,"-writeconfig")==0)//figureNumber=1
				{
					saveVarsToFile(argv[++argnum],xdecorations_rc);
					return(0);
				}
//print help
			if(strcmp(argstr,"-help")==0)
				doHelp();
		}

	srand((int)time((long* )NULL));

	signal(SIGKILL,(sighandler_t)&signalHandler);
	signal(SIGTERM,(sighandler_t)signalHandler);
	signal(SIGQUIT,(sighandler_t)signalHandler);
	signal(SIGINT,(sighandler_t)signalHandler);
	signal(SIGHUP,(sighandler_t)signalHandler);

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);

	if(useWindow==false)
		{
			rootWin=ToonGetRootWindow(display,screen,&parentWindow);
			visual=DefaultVisual(display,screen);
			gc=XCreateGC(display,rootWin,0,NULL);
		}	
	else
		{
			rc=get_argb_visual(&visual,&depth);
			if(rc==0)
				{
					XSetWindowAttributes attr;
					attr.colormap=XCreateColormap(display,DefaultRootWindow(display),visual,AllocNone);
					attr.border_pixel=0;
					attr.background_pixel=0;

					rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWColormap | CWBorderPixel | CWBackPixel,&attr);
					XSelectInput(display,rootWin,StructureNotifyMask);

					xa=XInternAtom(display,"_NET_WM_STATE",False);
					xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
					xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
					xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
					xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
					xa_prop[4]=XInternAtom(display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
					xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);

					xa=XInternAtom(display,"_NET_WM_STATE",False);
					if(xa!=None)
						XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,5);

					hints.flags=2;
					hints.decorations=0;
					XChangeProperty(display,rootWin,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

					gc=XCreateGC(display,rootWin,0,0);
					rg=XCreateRegion();
					XMapWindow(display,rootWin);
					XSync(display, False);
					
					XMoveWindow(display,rootWin,0,0);
					XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
				}
			else
				{
					rootWin=ToonGetRootWindow(display,screen,&parentWindow);
					visual=DefaultVisual(display,screen);
					gc=XCreateGC(display,rootWin,0,NULL);
					printf("Can't get ARGB, do you have a composite manager running\n");
				}
		}

	initLamps();
	initTree();
	initFigure();
	initFlyers();
	initFalling();

	XSetFillStyle(display,gc,FillSolid);

	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);

	while (!done)
		{
			while (XPending(display))
				XNextEvent(display,&ev);
	
			switch(ev.type)
				{
					case ClientMessage:
						if (ev.xclient.message_type == XInternAtom(display,"WM_PROTOCOLS",1) && (Atom)ev.xclient.data.l[0] == XInternAtom(display,"WM_DELETE_WINDOW",1))
						done=1;
						continue;
						
						break;
				}

			usleep(mainDelay);
			runCounter++;

			if(lampSet!=0)
				{
					if((runCounter % lampSpeed)==0)
						updateLamps();
				}

			if(showTree==true)
				{
					if((runCounter % treelampSpeed)==0)
						treeNeedsUpdate=true;

					if((runCounter % starSpeed)==0 && showStar==true)
						updateStar();
				}

			if(figureNumber!=0)
				{
					if((runCounter % figureSpeed)==0)
						figureNeedsUpdate=true;
				}

			if(showFlyers==true)
				{
					if((runCounter % flyersSpeed)==0)
						flyerNeedsUpdate=true;
				}

			if(showFalling==true)
				{
					if((runCounter % fallSpeed)==0)
						fallingNeedsUpdate=true;
				}

			eraseRects();
			drawTreeLamps();
			drawFigure();
			drawFlyers();
			drawLamps();
			drawFalling();
		}

	if(useWindow==false)
		XClearWindow(display,rootWin);
	XCloseDisplay(display);
	return(0);
}




