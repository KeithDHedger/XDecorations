/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:43:16 BST 2018 keithdhedger@gmail.com

 * This file (draw.cpp) is part of XDecorations.

 * XDecorations is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * XDecorations is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with XDecorations.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"
#include "routines.h"
#include "update.h"

int				windowid;
XErrorHandler	old_handler=(XErrorHandler)0;

void drawTreeLamps(void)
{
	if(showTree==false)
		return;

	XSetClipMask(display,gc,treePixmap[ONMASK]);
	XSetClipOrigin(display,gc,treeX,treeY);
	XCopyArea(display,treePixmap[ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);

	if(showTreeLamps==true)
		{
			XSetClipMask(display,gc,treeLampsPixmap[treeOnOff][ONMASK]);
			XCopyArea(display,treeLampsPixmap[treeOnOff][ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showStar==true)
		{
			XSetClipMask(display,gc,starPixmap[_SELECTPIXMAP(ONMASK,starOnOff)]);
			XCopyArea(display,starPixmap[_SELECTPIXMAP(ONPIXMAP,starOnOff)],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}

	if(showTinsel==true)
		{
			XSetClipMask(display,gc,tinselPixmap[ONMASK]);
			XCopyArea(display,tinselPixmap[ONPIXMAP],drawOnThis,gc,0,0,treeWidth,treeHeight,treeX,treeY);
		}
}

void drawFlyers(void)
{
	if(showFlyers==false)
		return;

	for(int j=0; j<numberOfFlyers; j++)
		{
			XSetClipMask(display,gc,*(flyersMove[j].object->mask[flyersMove[j].imageNum]));
			XSetClipOrigin(display,gc,flyersMove[j].x,flyersMove[j].y);
			XCopyArea(display,*(flyersMove[j].object->pixmap[flyersMove[j].imageNum]),drawOnThis,gc,0,0,flyersMove[j].object->w[0],flyersMove[j].object->h[0],flyersMove[j].x,flyersMove[j].y);

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
	if(figureNumber==0)
		return;

//			imlib_context_set_drawable(drawOnThis);
//			imlib_context_set_blend(0);
//			imlib_context_set_filter(NULL);
//			imlib_context_set_mask(figurePixmap[_SELECTPIXMAP(ONMASK,figureOnOff)]);
//			imlib_context_set_image(figureImage[figureOnOff]);
//			imlib_context_set_operation(IMLIB_OP_SUBTRACT);
//			imlib_render_image_on_drawable(figureX,figureY);

	XSetClipMask(display,gc,figurePixmap[_SELECTPIXMAP(ONMASK,figureOnOff)]);
	XSetClipOrigin(display,gc,figureX,figureY);
	XCopyArea(display,figurePixmap[_SELECTPIXMAP(ONPIXMAP,figureOnOff)],drawOnThis,gc,0,0,figureW,figureH,figureX,figureY);
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

void removeInvalidWindow(void)
{
	if(windowSnow[windowid].lasty!=NULL)
		free(windowSnow[windowid].lasty);
	if(windowSnow[windowid].pixmap!=0)
		XFreePixmap(display,windowSnow[windowid].pixmap);
	if(windowSnow[windowid].mask!=0)
		XFreePixmap(display,windowSnow[windowid].mask);
	if(windowSnow[windowid].maskgc!=0)
		XFreeGC(display,windowSnow[windowid].maskgc);

	windowSnow[windowid].pixmap=0;
	windowSnow[windowid].mask=0;
	windowSnow[windowid].maskgc=0;
	windowSnow[windowid].keepSettling=false;
	windowSnow[windowid].maxHeight=maxWindowHeight;
	windowSnow[windowid].lasty=NULL;
	windowSnow[windowid].wid=0;
	windowSnow[windowid].width=0;
	windowSnow[windowid].x=-1;
	windowSnow[windowid].y=-1;
	windowSnow[windowid].showing=false;
}

int ApplicationErrorHandler(Display *display,XErrorEvent *theEvent)
{
	removeInvalidWindow();
    return 0 ;
}

void drawWindowSnow(void)
{
	XWindowAttributes	attr;

	XSync(display, true) ;
	XFlush(display) ;
	old_handler=XSetErrorHandler(ApplicationErrorHandler) ;

	for(int j=0;j<MAXWINDOWS;j++)
		{
			if(windowSnow[j].showing==false)
				continue;

			if(windowSnow[j].wid>0)
				{
					windowid=j;
					XGetWindowAttributes(display,windowSnow[j].wid,&attr);
					XSetClipMask(display,gc,windowSnow[j].mask);
					XSetClipOrigin(display,gc,windowSnow[j].x,windowSnow[j].y-windowSnow[j].maxHeight-windowYOffset);
					XCopyArea(display,windowSnow[j].pixmap,drawOnThis,gc,0,0,windowSnow[j].width,windowSnow[j].maxHeight,windowSnow[j].x,windowSnow[j].y-windowSnow[j].maxHeight-windowYOffset);
				}
		}

	XFlush(display);
	XSync(display,false);
	XSetErrorHandler(old_handler);
}

void drawSettled(void)
{
	if(maxBottomHeight==0)
		return;

	XSetClipMask(display,gc,bottomSnow.mask);
	XSetClipOrigin(display,gc,0,displayHeight-bottomSnow.maxHeight-bottomYOffset);
	XCopyArea(display,bottomSnow.pixmap,drawOnThis,gc,0,0,displayWidth,bottomSnow.maxHeight,0,displayHeight-bottomSnow.maxHeight-bottomYOffset);
}

void drawFalling(void)
{
	if(showFalling==false)
		return;

	for(int j=0; j<numberOfFalling; j++)
		{
			XSetClipMask(display,gc,*(moving[j].object->mask[moving[j].imageNum]));
			XSetClipOrigin(display,gc,moving[j].x,moving[j].y);
			XCopyArea(display,*(moving[j].object->pixmap[moving[j].imageNum]),drawOnThis,gc,0,0,moving[j].object->w[0],moving[j].object->h[0],moving[j].x,moving[j].y);
//			imlib_context_set_blend(1);
//			imlib_context_set_image(moving[j].object->image);
//			imlib_render_image_on_drawable(moving[j].x,moving[j].y);
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
