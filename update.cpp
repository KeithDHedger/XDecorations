/******************************************************
*
*     ©keithhedger Wed 17 Dec 11:17:18 GMT 2014
*     kdhedger68713@gmail.com
*
*     update.cpp
* 
******************************************************/

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

#include "globals.h"
#include "routines.h"

void clearSettled(void)
{
	for(int j=0; j<displayWidth; j++)
		down[j]=settledHeight;

	XSetClipMask(display,gcpm,0);
	XSetClipOrigin(display,gcpm,0,0);

	XSetForeground(display,gcpm,blackColor);
	XSetFillStyle(display,gcpm,FillSolid);
	XFillRectangle(display,settledPixmapMask,gcpm,0,0,displayWidth,settledHeight);

	XSetClipMask(display,gc,0);
	XSetClipOrigin(display,gc,0,0);

	XSetForeground(display,gc,blackColor);
	XSetFillStyle(display,gc,FillSolid);
	XFillRectangle(display,settledPixmap,gc,0,0,displayWidth,settledHeight);
	keepSettling=true;
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
			clearSettled();
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
	int		downx;
	Pixmap	*pmm;

	for(int j=0; j<numberOfFalling; j++)
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
							if(keepSettling==true)
								{
									downx=moving[j].x;
									if((downx>=0) && (downx<displayWidth) && (settledHeight>0))
										{
											pmm=moving[j].object->mask[0];

											if(down[downx]-settleRate<=0)
												{
													if(clearOnMaxHeight==true)
														clearSettled();
													else
														down[downx]=settledHeight;
												}
											down[downx]=down[downx]-settleRate;
											if(down[downx]<=0)
												keepSettling=false;

											XSetClipMask(display,gcpm,*(pmm));
											XSetClipOrigin(display,gcpm,downx,down[downx]);
											XCopyArea(display,*(moving[j].object->mask[0]),settledPixmapMask,gcpm,0,0,moving[j].object->w[0],moving[j].object->h[0],downx,down[downx]);

											XSetClipMask(display,gc,*(pmm));
											XSetClipOrigin(display,gc,downx,down[downx]);
											XCopyArea(display,*(moving[j].object->pixmap[0]),settledPixmap,gc,0,0,moving[j].object->w[0],moving[j].object->h[0],downx,down[downx]);
										}
								}
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
