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
#include <X11/extensions/Xdbe.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#include "toon.h"

#define MAXPATHNAMELEN 4096
#define MAXNUMBEROFFLYERS 100
#define MAXFLYER 10
#define MAXNUMBEROFTREELIGHTS 10
#define MAXFLOAT 10
#define MAXFALLINGOBJECTS 5000
#define	MAXFALLINGANIMATION 48
#define MAXSWIRL 2

#define VERSION "0.1.5"
#define _SELECTPIXMAP(a,b) (a+(2*b))//a=ONPIXMAP b=xxxOnOff

enum {ONPIXMAP=0,ONMASK,OFFPIXMAP,OFFMASK};
enum {LEFT=-1000,CENTRE=-2000,RIGHT=-3000,TOP=-4000,BOTTOM=-5000};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {LAMPFLASH=0,INVERTLAMPCHASE,LAMPCHASE,LAMPRANDOM,LAMPCYCLE,NUMOPTIONS};

char				pathname[MAXPATHNAMELEN];
char*				configFilePath;

Display*			display;
Window				rootWin;
int					displayWidth;
int					displayHeight;
GC					gc;
Visual*				visual=NULL;
int					depth=0;
Imlib_Image			image;
int					screen;
Region				rg;
XdbeBackBuffer		buffer;
XdbeSwapInfo		swapInfo;
Drawable			drawOnThis;
bool				useBuffer=false;
bool				useDBOveride=true;

int					done=0;
long				mainDelay;

uint				runCounter=0;
bool				useWindow=true;
int					offSetY=0;
bool				watchConfig=false;

struct				objects
{
	Pixmap*			pixmap[MAXFALLINGANIMATION];
	Pixmap*			mask[MAXFALLINGANIMATION];
	int				h[MAXFALLINGANIMATION];
	int				w[MAXFALLINGANIMATION];
	int				anims;
};

struct				movement
{
	objects			*object;
	int				x;
	int				y;
	int				deltaX;
	int				deltaY;
	int				stepX;
	int				stepY;
	bool			use;
	int				imageNum;
	int				countDown;
	int				maxW;
	int				maxH;
	bool			direction;
};

//falling
objects				floating[MAXFLOAT];
movement			moving[MAXFALLINGOBJECTS];
int					fallingNumber=1;
int					fallingDelay=1;
int					swirlingDirection;
int					numberOfFalling=100;
bool				showFalling=true;
int					fallingSpread=1000;
int					fallSpeed=1;
int					minFallSpeed=1;
int					maxXStep=4;
int					fallingAnimSpeed=8;
int					fallingCount=0;

//wind
int					gustDuration=3000;
int					gustEvent=10000;
int					gustSpeed=40;
int					realGustSpeed;

bool				useGusts=true;
int					windSpeed=0;
int					gustOffSet=0;
int					gustPortion=1;
double				gustInc;
bool				doingGusts=false;
double				incr=1.01;

int					gustDirection=0;
int					gustCountdown=0;

//flyers
objects				flyers[MAXFLYER];
movement			flyersMove[MAXNUMBEROFFLYERS];
int					flyersSpeed=1;
int					flyersStep=8;
bool				showFlyers=false;
int					flyersMaxY=400;
//int					flyersActive[MAXNUMBEROFFLYERS];
int					flyerSpread=500;
int					flyerCount=0;
int					flyerAnimSpeed=8;
int					flyerNumber;
int					numberOfFlyers=1;

//figure
Pixmap				figurePixmap[4];
int					figureSpeed=100;
int					figureX=100;
int					figureY=100;
int					figureW;
int					figureH;
int					figureOnOff=0;
int					figureNumber=1;

//lamps
Pixmap				lampsPixmap[4];
int					lampSpeed=100;
int					lampX=0;
int					lampY=0;
int					lampWidth;
int					lampHeight;
int					lampCount;
int					lampSet=1;
int					lampsOnOff=0;
int					lampAnim;
int					lastLampAnim;
int					lampSection;
int					lampCountdown;
bool				*lampState;
int					lampCycleDelay;

//trees
Pixmap				treePixmap[2];
int					treeWidth;
int					treeHeight;
int					treeNumber=1;
int					treelampSpeed=100;
int					starSpeed=100;
int					treeX=0;
int					treeY=0;
int					treeLampSet=1;
int					treeOnOff=0;
bool				showTree=true;

//tinsel`
Pixmap				tinselPixmap[2];
bool				showTinsel=false;

//star
Pixmap				starPixmap[4];
bool				showStar=false;
int					starOnOff=0;

//tree lamps
Pixmap				treeLampsPixmap[MAXNUMBEROFTREELIGHTS][2];
bool				showTreeLamps=false;
int					treeLampCount=0;

char*				prefix;

bool				treeNeedsUpdate=false;
bool				flyerNeedsUpdate=false;
bool				figureNeedsUpdate=false;
bool				fallingNeedsUpdate=false;
bool				lampsNeedsUpdate=false;

struct args
{
	const char*		name;
	int				type;
	void*			data;
};

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

args				xdecorations_rc[]=
{
//app
	{"theme",TYPESTRING,&prefix},
	{"delay",TYPEINT,&mainDelay},
	{"usewindow",TYPEBOOL,&useWindow},
	{"usebuffer",TYPEBOOL,&useDBOveride},

//lamps
	{"lampset",TYPEINT,&lampSet},
	{"lampy",TYPEINT,&lampY},
	{"lampdelay",TYPEINT,&lampSpeed},
	{"lampflash",TYPEINT,&lampAnim},
	{"lampcycledelay",TYPEINT,&lampCycleDelay},

//flyers
	{"flyer",TYPEINT,&flyerNumber},
	{"maxflyers",TYPEINT,&numberOfFlyers},
	{"flyermaxy",TYPEINT,&flyersMaxY},
	{"spread",TYPEINT,&flyerSpread},
	{"flydelay",TYPEINT,&flyersSpeed},
	{"flystep",TYPEINT,&flyersStep},
	{"flyanimdelay",TYPEINT,&flyerAnimSpeed},

//trees
	{"tree",TYPEINT,&treeNumber},
	{"treex",TYPEINT,&treeX},
	{"treey",TYPEINT,&treeY},
	{"star",TYPEBOOL,&showStar},
	{"stardelay",TYPEINT,&starSpeed},
	{"tinsel",TYPEBOOL,&showTinsel},
	{"treelampset",TYPEINT,&treeLampSet},
	{"treelampdelay",TYPEINT,&treelampSpeed},

//figure
	{"figure",TYPEINT,&figureNumber},
	{"figurex",TYPEINT,&figureX},
	{"figurey",TYPEINT,&figureY},
	{"figuredelay",TYPEINT,&figureSpeed},

//falling
	{"falling",TYPEINT,&fallingNumber},
	{"falldelay",TYPEINT,&fallingDelay},
	{"maxfalling",TYPEINT,&numberOfFalling},
	{"fallingspread",TYPEINT,&fallingSpread},
	{"fallingspeed",TYPEINT,&minFallSpeed},
	{"minfallspeed",TYPEINT,&fallSpeed},
	{"maxxstep",TYPEINT,&maxXStep},

//wind
	{"wind",TYPEINT,&windSpeed},
	{"usegusts",TYPEBOOL,&useGusts},
	{"gustlen",TYPEINT,&gustDuration},
	{"gustdelay",TYPEINT,&gustEvent},
	{"gustspeed",TYPEINT,&gustSpeed},

	{NULL,0,NULL}
};

void setGravity(int *x,int *y,int w,int h)
{
	switch(*x)
		{
			case LEFT:
				*x=0;
			break;
			case CENTRE:
				*x=(displayWidth/2)-(w/2);
				break;
			case RIGHT:
				*x=displayWidth-w;
				break;
			default:
				break;
		}
	switch(*y)
		{
			case TOP:
				*y=0+offSetY;
			break;
			case CENTRE:
				*y=(displayHeight/2)-(h/2)+offSetY;
				break;
			case BOTTOM:
				*y=displayHeight-h+offSetY;
				break;
			default:
				break;
		}
}

void saveVarsToFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	if(filepath[0]=='-')
		fd=stdout;
	else
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
	int		flynumber;
	int		numfly;
	bool	randomize=true;

	if(flyerNumber==0)
		{
			showFlyers=false;
			return;
		}
	else
		showFlyers=true;

	flyerCount=0;

	if(numberOfFlyers<0)
		{
			numberOfFlyers=numberOfFlyers*-1;
			numfly=numberOfFlyers;
			randomize=false;
			flynumber=-1;
		}
	else
		{
			numfly=numberOfFlyers;
			randomize=true;
		}


	for(int j=0;j<MAXFLYER;j++)
		{
			flyers[flyerCount].anims=0;
			flyersMove[flyerCount].maxW=0;
			flyersMove[flyerCount].maxH=0;
			for(int k=0;k<MAXFALLINGANIMATION;k++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Flying/%i.%i.%i.png",DATADIR,prefix,flyerNumber,j+1,k+1);
					image=imlib_load_image(pathname);
					if(image!=NULL)
						{
							flyers[flyerCount].pixmap[k]=(Pixmap*)malloc(sizeof(Pixmap));
							flyers[flyerCount].mask[k]=(Pixmap*)malloc(sizeof(Pixmap));
							imlib_context_set_image(image);
							imlib_context_set_drawable(drawOnThis);
							imlib_image_set_has_alpha(1);
							imlib_render_pixmaps_for_whole_image(flyers[flyerCount].pixmap[k],flyers[flyerCount].mask[k]);
							flyers[flyerCount].w[k]=imlib_image_get_width();
							flyers[flyerCount].h[k]=imlib_image_get_height();
							flyers[flyerCount].anims++;
							if(flyers[flyerCount].w[k]>flyersMove[flyerCount].maxW)
								flyersMove[flyerCount].maxW=flyers[flyerCount].w[k];
							if(flyers[flyerCount].h[k]>flyersMove[flyerCount].maxH)
								flyersMove[flyerCount].maxH=flyers[flyerCount].h[k];
						}
				}
			if(flyers[flyerCount].anims!=0)
					flyerCount++;
		}

	if(flyerCount==0)
		showFlyers=false;
	else
		{
			showFlyers=true;
			for(int j=0;j<numfly;j++)
				{
					if(randomize==true)
						flynumber=randInt(flyerCount);
					else
						{
							flynumber++;
							if(flynumber==flyerCount)
								flynumber=0;
						}
					flyersMove[j].object=&flyers[flynumber];
					flyersMove[j].imageNum=0;
					flyersMove[j].countDown=flyerAnimSpeed;
					flyersMove[j].x=0-flyersMove[j].object->w[0];
					flyersMove[j].y=(rand() % flyersMaxY);
					flyersMove[j].use=false;
				}
		}

}

void initFigure(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.On.png",DATADIR,prefix,figureNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		figureNumber=0;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&figurePixmap[ONPIXMAP],&figurePixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.Off.png",DATADIR,prefix,figureNumber);
			image=imlib_load_image(pathname);
			if(image==NULL)
				figureNumber=0;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&figurePixmap[OFFPIXMAP],&figurePixmap[OFFMASK]);
					figureW=imlib_image_get_width();
					figureH=imlib_image_get_height();
					setGravity(&figureX,&figureY,figureW,figureH);
				}
		}
}

void initLamps(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Lamps/%i.On.png",DATADIR,prefix,lampSet);
	image=imlib_load_image(pathname);
	if(image==NULL)
		lampSet=0;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
//imlib_render_image_on_drawable_at_size(0, 0, 200, 200);
			imlib_render_pixmaps_for_whole_image(&lampsPixmap[ONPIXMAP],&lampsPixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Lamps/%i.Off.png",DATADIR,prefix,lampSet);
			image=imlib_load_image(pathname);
			if(image==NULL)
				lampSet=0;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
//imlib_render_image_on_drawable_at_size(0, 0, 200, 200);
					imlib_render_pixmaps_for_whole_image(&lampsPixmap[OFFPIXMAP],&lampsPixmap[OFFMASK]);

					lampWidth=imlib_image_get_width();
					lampHeight=imlib_image_get_height();
					lampCount=(displayWidth/lampWidth)+1;
				}
			lampState=(bool*)malloc(sizeof(bool)*lampCount);
			for(int j=0;j<lampCount;j++)
				lampState[j]=false;
	lampCountdown=lampCount;
	lampSection=0;
		}
}

void destroyFalling(void)
{
	for(int j=0;j<fallingCount;j++)
		{
			for(int k=0;k<floating[j].anims;k++)
				{
					imlib_free_pixmap_and_mask(*(floating[j].pixmap[k]));
				}
		}
}

void destroyFlyers(void)
{
	for(int j=0;j<flyerCount;j++)
		{
			for(int k=0;k<flyers[j].anims;k++)
				{
					imlib_free_pixmap_and_mask(*(flyers[j].pixmap[k]));
				}
		}
}


void initFalling(void)
{
	int	floatnumber;

	if(fallingNumber==0)
		{
			showFalling=false;
			return;
		}
	else
		showFalling=true;

	fallingCount=0;

	for(int j=0;j<MAXFLOAT;j++)
		{
			floating[fallingCount].anims=0;
			for(int k=0;k<MAXFALLINGANIMATION;k++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Falling/%i.%i.%i.png",DATADIR,prefix,fallingNumber,j+1,k+1);
					image=imlib_load_image(pathname);
					if(image!=NULL)
						{
							floating[fallingCount].pixmap[k]=(Pixmap*)malloc(sizeof(Pixmap));
							floating[fallingCount].mask[k]=(Pixmap*)malloc(sizeof(Pixmap));
							imlib_context_set_image(image);
							imlib_context_set_drawable(drawOnThis);
							imlib_image_set_has_alpha(1);
							imlib_render_pixmaps_for_whole_image(floating[fallingCount].pixmap[k],floating[fallingCount].mask[k]);
							floating[fallingCount].w[k]=imlib_image_get_width();
							floating[fallingCount].h[k]=imlib_image_get_height();
							floating[fallingCount].anims++;
						}
				}
			if(floating[fallingCount].anims!=0)
					fallingCount++;
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
					moving[j].y=0-moving[j].object->h[0];
					moving[j].deltaX=1;
					moving[j].deltaY=1;
					moving[j].stepX=1;
					moving[j].stepY=randInt(fallSpeed-minFallSpeed+1)+minFallSpeed;
					moving[j].use=false;
					moving[j].imageNum=randInt(moving[j].object->anims);
					moving[j].countDown=fallingAnimSpeed;
					moving[j].direction=randomEvent(2);
				}
		}
}

void initTree(void)
{
	bool	gotsomelamps;

	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.png",DATADIR,prefix,treeNumber);

	image=imlib_load_image(pathname);
	if(image==NULL)
		showTree=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&treePixmap[ONPIXMAP],&treePixmap[ONMASK]);
			treeWidth=imlib_image_get_width();
			treeHeight=imlib_image_get_height();
			setGravity(&treeX,&treeY,treeWidth,treeHeight);
		}

	treeLampCount=0;
	gotsomelamps=false;
	if(treeLampSet!=0)
		{
			showTreeLamps=true;
			for(int j=0; j<MAXNUMBEROFTREELIGHTS; j++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/Lights.%i.%i.png",DATADIR,prefix,treeLampSet,j+1);
					image=imlib_load_image(pathname);
					if(image!=NULL)
						{
							imlib_context_set_image(image);
							imlib_context_set_drawable(drawOnThis);
							imlib_image_set_has_alpha(1);
							imlib_render_pixmaps_for_whole_image(&treeLampsPixmap[j][ONPIXMAP],&treeLampsPixmap[j][ONMASK]);
							treeLampCount++;
							gotsomelamps=true;
						}
				}
		}
	if(gotsomelamps==false)
		showTreeLamps=false;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Star.Off.png",DATADIR,prefix,treeNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showStar=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&starPixmap[ONPIXMAP],&starPixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Star.On.png",DATADIR,prefix,treeNumber);
			image=imlib_load_image(pathname);
			if(image==NULL)
				showStar=false;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&starPixmap[OFFPIXMAP],&starPixmap[OFFMASK]);
				}
		}

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Tinsel.png",DATADIR,prefix,treeNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showTinsel=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&tinselPixmap[ONPIXMAP],&tinselPixmap[ONMASK]);
		}
}

void doGusts(void)
{
	if(doingGusts==false)
		{
			gustOffSet=0;
			return;
		}

	if(gustPortion==1)
		{
			gustInc=gustInc*incr;
			if(realGustSpeed<gustSpeed)
				realGustSpeed=realGustSpeed+gustInc;
			else
				{
					gustPortion=2;
					realGustSpeed=gustSpeed;
				}
			gustOffSet=realGustSpeed;
		}

	if(gustPortion==2)
		{
			gustCountdown--;
			realGustSpeed=gustSpeed;
			gustOffSet=realGustSpeed;
			if(gustCountdown<=0)
				{
					gustPortion=3;
					gustCountdown=0;
				}
		}

	if(gustPortion==3)
		{
			gustInc=gustInc/incr;
			if(gustInc<1)
				gustInc=1;
			if(realGustSpeed>0)
				realGustSpeed=realGustSpeed-(int)gustInc;
			else
				{
					gustPortion=1;
					gustCountdown=0;
					realGustSpeed=0;
					doingGusts=false;
					gustOffSet=0;
				}
			gustOffSet=realGustSpeed;
		}
}

void drawFlyers(void)
{
	int rc;

	if(showFlyers==false)
		return;

	for(int j=0;j<numberOfFlyers;j++)
		{
			rc=XSetClipMask(display,gc,*(flyersMove[j].object->mask[flyersMove[j].imageNum]));
			rc=XSetClipOrigin(display,gc,flyersMove[j].x,flyersMove[j].y);
			rc=XCopyArea(display,*(flyersMove[j].object->pixmap[flyersMove[j].imageNum]),drawOnThis,gc,0,0,flyersMove[j].object->w[0],flyersMove[j].object->h[0],flyersMove[j].x,flyersMove[j].y);

			flyersMove[j].countDown--;
			if(flyersMove[j].countDown==0)
				{
					flyersMove[j].countDown=flyerAnimSpeed;
					flyersMove[j].imageNum++;
					if(flyersMove[j].imageNum==flyersMove[j].object->anims)
						flyersMove[j].imageNum=0;
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
	rc=XCopyArea(display,figurePixmap[_SELECTPIXMAP(ONPIXMAP,figureOnOff)],drawOnThis,gc,0,0,figureW,figureH,figureX,figureY);
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

	switch(lastLampAnim)
		{
			case LAMPFLASH:
				for (loop=0; loop<lampCount; loop++)
					{
						rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
						rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,lampsOnOff)],drawOnThis,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
						CurrentlampX+=lampWidth;
					}
				break;

			case LAMPRANDOM:
				if(lampsNeedsUpdate==true)
					{
						lampsNeedsUpdate=false;
						for (loop=0; loop<lampCount; loop++)
							{
								lampState[loop]=randomEvent(2);
							}
					}
				for (loop=0; loop<lampCount; loop++)
					{
						rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
						rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,(int)lampState[loop])],drawOnThis,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
						CurrentlampX+=lampWidth;
					}
				break;

			case LAMPCHASE:
				if(lampsNeedsUpdate==true)
					{
						lampsNeedsUpdate=false;
						for (loop=0; loop<lampCount; loop++)
							{
								lampState[loop]=false;
							}
						lampSection++;
						if(lampSection==lampCount)
							lampSection=0;
						lampState[lampSection]=true;
					}

				for (loop=0; loop<lampCount; loop++)
					{
						rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
						rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,(int)lampState[loop])],drawOnThis,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
						CurrentlampX+=lampWidth;
					}
				break;

			case INVERTLAMPCHASE:
				if(lampsNeedsUpdate==true)
					{
						lampsNeedsUpdate=false;
						for (loop=0; loop<lampCount; loop++)
							{
								lampState[loop]=true;
							}
						lampSection++;
						if(lampSection==lampCount)
							lampSection=0;
						lampState[lampSection]=false;
					}

				for (loop=0; loop<lampCount; loop++)
					{
						rc+=XSetClipOrigin(display,gc,CurrentlampX,lampY);
						rc+=XCopyArea(display,lampsPixmap[_SELECTPIXMAP(ONPIXMAP,(int)lampState[loop])],drawOnThis,gc,0,0,lampWidth,lampHeight,CurrentlampX,lampY);
						CurrentlampX+=lampWidth;
					}
				break;

		}
}

void drawFalling(void)
{
	int rc;

	if(showFalling==false)
		return;

	for(int j=0;j<numberOfFalling;j++)
		{
			rc=XSetClipMask(display,gc,*(moving[j].object->mask[moving[j].imageNum]));
			rc=XSetClipOrigin(display,gc,moving[j].x,moving[j].y);
			rc=XCopyArea(display,*(moving[j].object->pixmap[moving[j].imageNum]),drawOnThis,gc,0,0,moving[j].object->w[0],moving[j].object->h[0],moving[j].x,moving[j].y);
			moving[j].countDown--;
			if(moving[j].countDown==0)
				{
					moving[j].countDown=fallingAnimSpeed;
					if(moving[j].direction==true)
						{
							moving[j].imageNum++;
							if(moving[j].imageNum==moving[j].object->anims)
								moving[j].imageNum=0;
						}
					else
						{
							moving[j].imageNum--;
							if(moving[j].imageNum<0)
								moving[j].imageNum=moving[j].object->anims-1;
						}
				}
	}
}

void updateGusts(void)
{
	if(doingGusts==false)
		{
			if((randomEvent(gustEvent)==true) && (gustPortion==1))
				{
					gustCountdown=gustDuration;
					gustDirection=randomDirection();
					realGustSpeed=1;
					gustPortion=1;
					gustInc=1.0;
					doingGusts=true;
				}
		}
	else
		doGusts();

}

void updateFalling(void)
{
	for(int j=0;j<numberOfFalling;j++)
		{
			if(moving[j].use==true)
				{
					swirlingDirection=randInt(MAXSWIRL);
					if(randomEvent(2)==true)
						swirlingDirection=-swirlingDirection;
					moving[j].stepX=moving[j].stepX+swirlingDirection;
					if(moving[j].stepX>maxXStep)
						moving[j].stepX=maxXStep;
					if(moving[j].stepX<-maxXStep)
						moving[j].stepX=-maxXStep;

					moving[j].y=moving[j].y+moving[j].stepY;
					moving[j].x=moving[j].x+moving[j].stepX+windSpeed+(gustOffSet*gustDirection);

					if(moving[j].y>displayHeight+moving[j].object->h[0])
						{
							moving[j].use=false;
							moving[j].y=0-moving[j].object->h[0];
						}

					if(moving[j].x>displayWidth+moving[j].object->w[0])
						moving[j].x=0-moving[j].object->w[0];

					if(moving[j].x<0-moving[j].object->w[0])
						moving[j].x=displayWidth;
				}
			else
				{
					if(randomEvent(fallingSpread)==true)
						{
							moving[j].use=true;
							moving[j].stepY=randInt(fallSpeed-minFallSpeed+1)+minFallSpeed;
							moving[j].x=(rand() % displayWidth);
							moving[j].imageNum=randInt(moving[j].object->anims);
							moving[j].countDown=fallingAnimSpeed;
							moving[j].direction=randomEvent(2);
						}
				}
		}
}

void updateLamps(void)
{
	lampsOnOff=(lampsOnOff+1) & 1;
	lampsNeedsUpdate=true;

	if(lampCountdown<1)
		{
			if((lampAnim==LAMPCYCLE) && (lampsOnOff==1))
				{
					lastLampAnim++;

					if(lastLampAnim==LAMPCYCLE)
						lastLampAnim=LAMPFLASH;

					lampCountdown=lampCycleDelay;
				}
		}
	else
		lampCountdown--;
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
	rc=XCopyArea(display,treePixmap[ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if(showTreeLamps==true)
		{
			rc=XSetClipMask(display,gc,treeLampsPixmap[treeOnOff][ONMASK]);
			rc=XCopyArea(display,treeLampsPixmap[treeOnOff][ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showStar==true)
		{
			rc=XSetClipMask(display,gc,starPixmap[_SELECTPIXMAP(ONMASK,starOnOff)]);
			rc=XCopyArea(display,starPixmap[_SELECTPIXMAP(ONPIXMAP,starOnOff)],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showTinsel==true)
		{
			rc=XSetClipMask(display,gc,tinselPixmap[ONMASK]);
			rc=XCopyArea(display,tinselPixmap[ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
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

	for(j=0; j<numberOfFlyers; j++)
		{
			if(flyersMove[j].use==true)
				{
					flyersMove[j].x+=flyersStep+((gustOffSet*gustDirection)/4);

					if(flyersMove[j].x>displayWidth+flyersMove[j].object->w[0])
						{
							flyersMove[j].y=(rand() % flyersMaxY);
							flyersMove[j].x=0-flyersMove[j].object->w[0];
							flyersMove[j].use=false;						
						}
				}
			else
				{
					if(randomEvent(flyerSpread)==true)
						flyersMove[j].use=true;
				}
			
		}
}

void eraseRects(void)
{
	int	rc=0;
	int	j;

	if((figureNumber!=0) && (figureNeedsUpdate==true))
		{
			if(useBuffer==false)
				rc=XClearArea(display,drawOnThis,figureX,figureY,figureW,figureH,False);
			updateFigure();
		}

	if((showTree==true) && (treeNeedsUpdate==true))
		{
			if(useBuffer==false)
				rc=XClearArea(display,drawOnThis,treeX,treeY,treeWidth,treeHeight,False);
			updateTreeLamps();
		}

	if((showFlyers==true) && (flyerNeedsUpdate==true))
		{
			if(useBuffer==false)
				{
					for(j=0; j<flyerCount; j++)
						rc=XClearArea(display,drawOnThis,flyersMove[j].x,flyersMove[j].y,flyersMove[j].maxW,flyersMove[j].maxH,False);
				}
			updateFlyers();
		}

	if((showFalling==true) && (fallingNeedsUpdate==true))
		{
			if(useBuffer==false)
				{
					for(int j=0;j<numberOfFalling;j++)
						rc=XClearArea(display,drawOnThis,moving[j].x,moving[j].y,moving[j].object->w[0],moving[j].object->h[0],False);
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
	visual_list = XGetVisualInfo (display,0,&visual_template, &nxvisuals);

	for (i = 0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth == 32 &&
			        (visual_list[i].red_mask   == 0xff0000 &&
			         visual_list[i].green_mask == 0x00ff00 &&
			         visual_list[i].blue_mask  == 0x0000ff ))
				{
					*vis = visual_list[i].visual;
					*depth = visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

void doHelp(void)
{
	printf("XDecorations (c)2014 K. D. Hedger - Version %s\n",VERSION);
	printf("Released under the gpl-3.0 license\n\n");
	printf("Values are set to defaults then set to values contained in ~/.config/xdecorations.rc and then overridden on the command line\n\n");

//app
	printf("APPLICATION OPTIONS\n");
	printf("-theme STRING\n");
	printf("\tSet theme name ( default 'Xmas' )\n");
	printf("-delay INTEGER\n");
	printf("\tSet main delay\n");
	printf("-configfile FILEPATH\n");
	printf("\tSet new config file ( using only one instance of this makes any sense )\n");
	printf("-writeconfig FILEPATH\n");
	printf("\tWrite out a new config file including currently loaded config file,defaults and command line options then exit\n");
	printf("\tOptions after this are ignored\n");
	printf("-noconfig\n");
	printf("\tDont't load the default config file ( ~/.config/xdecorations.rc )\n");
	printf("-usewindow/-no-usewindow\n");
	printf("\tUse a transparent window instead of the root window\n");
	printf("-usebuffer/-no-usebuffer\n");
	printf("\tUse double buffering\n\tUsing double buffering gives best graphical results but is slower\n");
	printf("\tThe default is to use a transparent window with double buffering\n\n");
//lamps
	printf("LAMPS\n");
	printf("-lampset INTEGER\n");
	printf("\tThe number of the lamp set to use ( 0=no lamps )\n");
	printf("-lampy INTEGER\n");
	printf("\tLamp Y position\n");
	printf("-lampflash INTEGER\n");
	printf("\tLamp flash type 0=flash, 1=inverted chase, 2=chase ,3=random, 4=cycle\n");
	printf("-lampcycledelay INTEGER\n");
	printf("\tDelay before cycling to the next flash type\n\n");

//flying
	printf("FLYERS\n");
	printf("-flyer\n");
	printf("\tFlying object set ( 0=disable flyers )\n");
	printf("-maxflyers INTEGER\n");
	printf("\tNumber of flying objects to use <%i\n",MAXNUMBEROFFLYERS);
	printf("\tIf the number of flyers are > 0 then the image is chosen randomly from the available flyers\n");
	printf("\tIf the number of flyers are < 0 then tthe image is chosen sequentially\n");
	printf("-flyermaxy INTEGER\n");
	printf("\tLowest point on screen for flying objects\n");
	printf("-spread INTEGER\n");
	printf("\tRandom delay for flying objects\n");
	printf("-flydelay INTEGER\n");
	printf("\tFlying objects delay\n");
	printf("-flystep INTEGER\n");
	printf("\tAmount to move flying objects\n");
	printf("-flyanimdelay INTEGER\n");
	printf("\tDelay for animation of flying objects\n\n");

//tree
	printf("TREES\n");
	printf("-tree INTEGER\n");
	printf("\tThe number of the tree to use ( 0=no tree )\n");
	printf("\tIf no trees are shown star/treelights/tinsel are also disabled\n");
	printf("-treex INTEGER\n");
	printf("\tAbsolute X position of tree\n");
	printf("-treey INTEGER\n");
	printf("\tAbsolute Y position of tree\n");
	printf("\tYou can also use the terms left/centre/right with treex to position the tree\n");
	printf("\tYou can also use the terms top/centre/bottom with treey to position the tree\n");
//star
	printf("-showstar/-no-showstar\n");
	printf("\tShow star\n");
	printf("-stardelay INTEGER\n");
	printf("\tDelay for star\n");
//tinsel
	printf("-showtinsel/-no-showtinsel\n");
	printf("\tShow tinsel\n");
//treelamps
	printf("-treelampset INTEGER\n");
	printf("\tLampset to use on tree\n");
	printf("-treelampdelay INTEGER\n");
	printf("\tTree lamps delay\n\n");

//figure
	printf("FIGURE\n");
	printf("-figure INTEGER\n");
	printf("\tThe number of figure to use ( 0=no figure )\n");
	printf("-figurex INTEGER\n");
	printf("\tAbsolute X position of figure\n");
	printf("-figurey INTEGER\n");
	printf("\tAbsolute Y position of figure\n");
	printf("-figuredelay INTEGER\n");
	printf("\tDelay for figure\n");
	printf("\tYou can also use the terms left/centre/right with figurex to position the figure\n");
	printf("\tYou can also use the terms top/centre/bottom with figurey to position the figure\n\n");

//falling
	printf("FALLING OBJECTS\n");
	printf("-falling INTEGER\n");
	printf("\tThe set number of the falling objects to use ( 0=no falling objects )\n");
	printf("-falldelay INTEGER\n");
	printf("\tDelay for falling objects\n");
	printf("-maxfalling INTEGER\n");
	printf("\tMaximum number of falling objects INTEGER<%i\n",MAXFALLINGOBJECTS);
	printf("-fallingspread INTEGER\n");
	printf("\tRandom deleay between new falling objects appearing\n");
	printf("-fallingspeed INTEGER\n");
	printf("\tDistance falling objects move in Y direction in one go\n");
	printf("-minfallspeed INTEGER\n");
	printf("\tMin distance falling objects move in Y direction in one go ( MUST be less than fallingspeed )\n");
	printf("-maxxstep INTEGER\n");
	printf("\tFalling objects max X step\n");
	printf("-fallinanimdelay INTEGER\n");
	printf("\tDelay for animation of falling objects\n\n");

//wind
	printf("WIND\n");
	printf("-wind INTEGER\n");
	printf("\tWind speed/direction ( +INTEGER wind>>> -INTEGER wind<<< 0 no wind )\n");
//gusts
	printf("-usegusts/-no-usegusts\n");
	printf("\tUse/don't use gusts of wind\n");
	printf("-gustlen INTEGER\n");
	printf("\tDuration of gusts of wind\n");
	printf("-gustdelay INTEGER\n");
	printf("\tRandom delay between gusts of wind\n");
	printf("-gustspeed INTEGER\n");
	printf("\tSpeed of gusts of wind\n");
	printf("\n");
	exit(0);
}

void setDefaults(void)
{
	prefix=strdup("Xmas");
	treeNumber=0;
	figureNumber=1;
	showFlyers=false;
	fallingNumber=1;
	fallingSpread=2000;
	lampSpeed=20;
	gustEvent=1500;
	gustDuration=100;
	gustSpeed=60;
	gustOffSet=0;
	lampsNeedsUpdate=true;
	lampSection=0;
	lampAnim=LAMPFLASH;
	lastLampAnim=LAMPFLASH;
	lampCycleDelay=30;
	lampCountdown=lampCycleDelay;
	mainDelay=25000;
	fallSpeed=1;
	minFallSpeed=1;
	flyerNumber=0;
	numberOfFlyers=20;
}

int translateGravity(char* str)
{
	int retcode=0;

	if(isalpha(str[0]))
		{
			if(strcasecmp(str,"left")==0)
				retcode=LEFT;
			if(strcasecmp(str,"centre")==0)
				retcode=CENTRE;
			if(strcasecmp(str,"right")==0)
				retcode=RIGHT;
			if(strcasecmp(str,"top")==0)
				retcode=TOP;
			if(strcasecmp(str,"bottom")==0)
				retcode=BOTTOM;
		}
	else
		retcode=atol(str);

	return(retcode);

}

void reloadConfig(void)
{
	int numflyobjects=numberOfFlyers;
	int flysetnumber=flyerNumber;
	int numfallobjects=numberOfFalling;
	int fallsetnumber=fallingNumber;

	if(numflyobjects<0)
		numflyobjects=numflyobjects*-1;

	loadVarsFromFile(configFilePath,xdecorations_rc);

	if((numflyobjects<numberOfFlyers) || (flysetnumber!=flyerNumber))
		{
			destroyFlyers();
			initFlyers();
		}

	if((numfallobjects<numberOfFalling) || (fallsetnumber!=fallingNumber))
		{
			destroyFalling();
			initFalling();
		}
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

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);

	setDefaults();

	if((argc>1) && (strcmp(argv[1],"-noconfig")!=0))
		{
			prefix=strdup("Xmas");
			asprintf(&configFilePath,"%s/.config/xdecorations.rc",getenv("HOME"));
			loadVarsFromFile(configFilePath,xdecorations_rc);
		}

//command line options.
	for (argnum=1; argnum<argc; argnum++)
		{
			argstr=argv[argnum];


//app
			if(strcmp(argstr,"-configfile")==0)//~/.config/xdecorations.rc
				{
					free(configFilePath);
					asprintf(&configFilePath,"%s",argv[++argnum]);
					loadVarsFromFile(configFilePath,xdecorations_rc);
				}

			if(strcmp(argstr,"-writeconfig")==0)//figureNumber=1
				{
					saveVarsToFile(argv[++argnum],xdecorations_rc);
					return(0);
				}

			showUnShow(argstr,"usewindow",&useWindow);//use/don't use window
			showUnShow(argstr,"usebuffer",&useDBOveride);//use/don'tdouble buffering
			showUnShow(argstr,"watchconfig",&watchConfig);//use/don'tdouble buffering
			if(strcmp(argstr,"-theme")==0)//Xmas
				{
					free(prefix);
					prefix=strdup(argv[++argnum]);
				}

			if(strcmp(argstr,"-delay")==0)//mainDelay=20000
				mainDelay=atol(argv[++argnum]);

			if(strcmp(argstr,"-offsety")==0)//offSetY=0
				offSetY=atol(argv[++argnum]);

//lamps
			if(strcmp(argstr,"-lampset")==0)//lampSet=1
				lampSet=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampy")==0)//lampY=16
				lampY=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampdelay")==0)//lampSpeed=100
				lampSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-lampflash")==0)//lampAnim=LAMPFLASH
				{
					lampAnim=atol(argv[++argnum]);
					lastLampAnim=lampAnim;
					if(lampAnim==LAMPCYCLE)
						lastLampAnim=LAMPFLASH;
				}
			if(strcmp(argstr,"-lampcycledelay")==0)//lampCycleDelay=30
				lampCycleDelay=atol(argv[++argnum]);

//flyers
			if(strcmp(argstr,"-flyer")==0)//flyersMaxY=400
				flyerNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxflyers")==0)//flyersMaxY=400
				numberOfFlyers=atol(argv[++argnum]);
			if(strcmp(argstr,"-flyermaxy")==0)//flyersMaxY=400
				flyersMaxY=atol(argv[++argnum]);
			if(strcmp(argstr,"-spread")==0)//flyerSpread=500
				flyerSpread=atol(argv[++argnum]);
			if(strcmp(argstr,"-flydelay")==0)//flyersSpeed=1
				flyersSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-flystep")==0)//flyersStep=8
				flyersStep=atol(argv[++argnum]);
			if(strcmp(argstr,"-flyanimdelay")==0)//flyersStep=8
				flyerAnimSpeed=atol(argv[++argnum]);

//tree
			if(strcmp(argstr,"-tree")==0)//treeNumber=1
				treeNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-treex")==0)//treeX=100
				treeX=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-treey")==0)//treeY=100
				treeY=translateGravity(argv[++argnum]);

			showUnShow(argstr,"showstar",&showStar);//showStar=false
			if(strcmp(argstr,"-stardelay")==0)//starSpeed
				starSpeed=atol(argv[++argnum]);

			showUnShow(argstr,"showtinsel",&showTinsel);//showTinsel=false

			if(strcmp(argstr,"-treelampset")==0)//treeLampSet=1
				treeLampSet=atol(argv[++argnum]);
			if(strcmp(argstr,"-treelampdelay")==0)//treelampSpeed=100
				treelampSpeed=atol(argv[++argnum]);

//figure
			if(strcmp(argstr,"-figure")==0)//figure=1
				figureNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-figurex")==0)//figureX=100
				figureX=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-figurey")==0)//figureY=100
					figureY=translateGravity(argv[++argnum]);
			if(strcmp(argstr,"-figuredelay")==0)//figureSpeed=100
				figureSpeed=atol(argv[++argnum]);

//falling
			if(strcmp(argstr,"-falling")==0)//falling set=1
				fallingNumber=atol(argv[++argnum]);
			if(strcmp(argstr,"-falldelay")==0)//falling speed=10
				fallingDelay=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxfalling")==0)//numberOfFalling=100
				numberOfFalling=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallingspread")==0)//leaf spread=400
				fallingSpread=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallingspeed")==0)//leaf spread=400
				fallSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-minfallspeed")==0)//max xstep =4
				minFallSpeed=atol(argv[++argnum]);
			if(strcmp(argstr,"-maxxstep")==0)//max xstep =4
				maxXStep=atol(argv[++argnum]);
			if(strcmp(argstr,"-fallinanimdelay")==0)//max xstep =4
				fallingAnimSpeed=atol(argv[++argnum]);

//wind
			if(strcmp(argstr,"-wind")==0)//windspeed=4
				windSpeed=atol(argv[++argnum]);

			showUnShow(argstr,"usegusts",&useGusts);//use/don't use gusts of wind
			if(strcmp(argstr,"-gustlen")==0)//gustDuration=10
				gustDuration=atol(argv[++argnum]);
			if(strcmp(argstr,"-gustdelay")==0)//gustEvent=2000
				gustEvent=atol(argv[++argnum]);
			if(strcmp(argstr,"-gustspeed")==0)//gustSpeed=40
				gustSpeed=atol(argv[++argnum]);

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

	if(useWindow==false)
		{
			rootWin=ToonGetRootWindow(display,screen,&parentWindow);
			visual=DefaultVisual(display,screen);
			useBuffer=false;
			drawOnThis=rootWin;
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

					rg=XCreateRegion();
					XMapWindow(display,rootWin);
					XSync(display, False);

					XMoveWindow(display,rootWin,0,0);
					XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);

					buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
					swapInfo.swap_window = rootWin;
					swapInfo.swap_action = XdbeBackground;
					if((XdbeSwapBuffers(display,&swapInfo,1)) && (useDBOveride==true))
					//if(false && (useDBOveride==true))
						{
							printf("got double buffer\n");
							useBuffer=true;
							drawOnThis=buffer;
						}
					else
						{
							printf("no double buffering\n");
							useBuffer=false;
							drawOnThis=rootWin;
						}
				}
			else
				{
					rootWin=ToonGetRootWindow(display,screen,&parentWindow);
					visual=DefaultVisual(display,screen);
					printf("Can't get ARGB, do you have a composite manager running\n");
					drawOnThis=rootWin;
					useBuffer=false;
				}
		}

	gc=XCreateGC(display,drawOnThis,0,NULL);

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
					if((runCounter % fallingDelay)==0)
						fallingNeedsUpdate=true;
				}

			if(useGusts==true)
				{
						updateGusts();
				}

			eraseRects();
			drawTreeLamps();
			drawFigure();
			drawFlyers();
			drawLamps();
			drawFalling();

			if(useBuffer==true)
				XdbeSwapBuffers(display,&swapInfo,1);

			if(watchConfig==true)
				reloadConfig();

		}
	if(useWindow==false)
		XClearWindow(display,rootWin);
	XCloseDisplay(display);

	destroyFalling();
	destroyFlyers();
	return(0);
}




