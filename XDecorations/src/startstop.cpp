/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:44:47 BST 2018 keithdhedger@gmail.com

 * This file (startstop.cpp) is part of XDecorations.

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
#include <unistd.h>

Imlib_Image		image;

void initBottomSnow(void)
{

	if(maxBottomHeight==0)
		return;

	bottomSnow.pixmap=XCreatePixmap(display,drawOnThis,displayWidth,maxBottomHeight+1,depth);
	bottomSnow.mask=XCreatePixmap(display,drawOnThis,displayWidth,maxBottomHeight+1,1);
	bottomSnow.maskgc=XCreateGC(display,bottomSnow.mask,0,NULL);
	bottomSnow.keepSettling=true;
	bottomSnow.maxHeight=maxBottomHeight+1;
	bottomSnow.lasty=(int*)malloc(sizeof(int)*displayWidth);
	bottomSnow.wid=0;
	bottomSnow.width=displayWidth;
	bottomSnow.x=0;
	bottomSnow.y=0;
}

void initWindowSnow(void)
{
	if(maxWindowHeight==0)
		return;

	for(int j=0;j<MAXWINDOWS;j++)
		{
			windowSnow[j].pixmap=0;
			windowSnow[j].mask=0;
			windowSnow[j].maskgc=0;
			windowSnow[j].keepSettling=false;
			windowSnow[j].maxHeight=maxWindowHeight+1;
			windowSnow[j].lasty=NULL;
			windowSnow[j].wid=0;
			windowSnow[j].width=0;
			windowSnow[j].x=-1;
			windowSnow[j].y=-1;
			windowSnow[j].showing=false;
		}
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
			numberOfFlyers=abs(numberOfFlyers);
			numfly=numberOfFlyers;
			randomize=false;
			flynumber=-1;
		}
	else
		{
			numfly=numberOfFlyers;
			randomize=true;
		}

	for(int j=0; j<MAXFLYER; j++)
		{
			flyers[flyerCount].anims=0;
			flyersMove[flyerCount].maxW=0;
			flyersMove[flyerCount].maxH=0;
			for(int k=0; k<MAXFALLINGANIMATION; k++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Flying/%i.%i.%i.png",pixmapPath,prefix,flyerNumber,j+1,k+1);
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
							imlib_free_image();
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
			for(int j=0; j<numfly; j++)
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

void initFigurezzz(void)
{
//	imlib_context_set_dither(0);
//	imlib_context_set_display(display);
//	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.On.png",pixmapPath,prefix,figureNumber);
	figureImage[0]=imlib_load_image(pathname);
	if(figureImage[0]==NULL)
		figureNumber=0;
	else
		{
//			
			imlib_context_set_image(figureImage[0]);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&figurePixmap[ONPIXMAP],&figurePixmap[ONMASK]);
//			imlib_free_image();
//
			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.Off.png",pixmapPath,prefix,figureNumber);
			figureImage[1]=imlib_load_image(pathname);
			if(figureImage[1]==NULL)
				figureNumber=0;
			else
				{
					imlib_context_set_image(figureImage[1]);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&figurePixmap[OFFPIXMAP],&figurePixmap[OFFMASK]);
					figureW=imlib_image_get_width();
					figureH=imlib_image_get_height();
					setGravity(&figureX,&figureY,figureW,figureH);
					//imlib_free_image();
				}
		}
}

void initFigure(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.On.png",pixmapPath,prefix,figureNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		figureNumber=0;
	else
		{
			
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&figurePixmap[ONPIXMAP],&figurePixmap[ONMASK]);
			imlib_free_image();

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Figure/%i.Off.png",pixmapPath,prefix,figureNumber);
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
					imlib_free_image();
				}
		}
}

void initLamps(void)
{
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Lamps/%i.On.png",pixmapPath,prefix,lampSet);
	
	image=imlib_load_image(pathname);
	if(image==NULL)
		lampSet=0;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&lampsPixmap[ONPIXMAP],&lampsPixmap[ONMASK]);

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Lamps/%i.Off.png",pixmapPath,prefix,lampSet);
			image=imlib_load_image(pathname);
			if(image==NULL)
				lampSet=0;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&lampsPixmap[OFFPIXMAP],&lampsPixmap[OFFMASK]);

					lampWidth=imlib_image_get_width();
					lampHeight=imlib_image_get_height();
					lampCount=(displayWidth/lampWidth)+1;
				}
			lampState=(bool*)malloc(sizeof(bool)*lampCount);
			for(int j=0; j<lampCount; j++)
				lampState[j]=false;
			lampCountdown=lampCount;
			lampSection=0;

			lastLampAnim=lampAnim;
			if(lampAnim==LAMPCYCLE)
				lastLampAnim=LAMPFLASH;
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

	for(int j=0; j<MAXFLOAT; j++)
		{
			floating[fallingCount].anims=0;
			for(int k=0; k<MAXFALLINGANIMATION; k++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Falling/%i.%i.%i.png",pixmapPath,prefix,fallingNumber,j+1,k+1);
					image=imlib_load_image(pathname);
					if(image!=NULL)
						{
							floating[fallingCount].pixmap[k]=(Pixmap*)malloc(sizeof(Pixmap));
							floating[fallingCount].mask[k]=(Pixmap*)malloc(sizeof(Pixmap));
							imlib_context_set_image(image);
							imlib_image_set_has_alpha(1);
							imlib_context_set_drawable(drawOnThis);
							imlib_render_pixmaps_for_whole_image(floating[fallingCount].pixmap[k],floating[fallingCount].mask[k]);
							floating[fallingCount].w[k]=imlib_image_get_width();
							floating[fallingCount].h[k]=imlib_image_get_height();
							floating[fallingCount].anims++;
//							floating[fallingCount].image=image;
							imlib_free_image();
							//floating[fallingCount].mask[k]=floating[fallingCount].pixmap[k];
//							GC fgc=XCreateGC(display,*(floating[fallingCount].pixmap[k]),0,NULL);
//							fgc=gc;
//							XSetClipMask(display,fgc,0);
//							XSetClipOrigin(display,fgc,0,0);
//
//			//				XSetForeground(display,fgc,blackColor);
//							XSetForeground(display,fgc,whiteColor);
//							XSetFillStyle(display,fgc,FillSolid);
//							XFillRectangle(display,*(floating[fallingCount].pixmap[k]),fgc,0,0,floating[fallingCount].w[k],floating[fallingCount].h[k]);
//
//fgc=XCreateGC(display,figurePixmap[OFFMASK],0,NULL);
//			XSetClipMask(display,fgc,0);
//			XSetClipOrigin(display,fgc,0,0);
//
//			//XSetForeground(display,fgc,blackColor);
//			XSetForeground(display,fgc,whiteColor);
//			XSetFillStyle(display,fgc,FillSolid);
//			XFillRectangle(display,figurePixmap[OFFMASK],fgc,0,0,277,512);

							
							
						}
				}
			if(floating[fallingCount].anims!=0)
				fallingCount++;
		}

	if(fallingCount==0)
		showFalling=false;
	else
		{
			for(int j=0; j<numberOfFalling; j++)
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

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.png",pixmapPath,prefix,treeNumber);

	image=imlib_load_image(pathname);
	if(image==NULL)
		showTree=false;
	else
		{
			showTree=true;
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&treePixmap[ONPIXMAP],&treePixmap[ONMASK]);
			treeWidth=imlib_image_get_width();
			treeHeight=imlib_image_get_height();
			setGravity(&treeX,&treeY,treeWidth,treeHeight);
			imlib_free_image();
		}

	treeLampCount=0;
	gotsomelamps=false;
	if(treeLampSet!=0)
		{
			showTreeLamps=true;
			for(int j=0; j<MAXNUMBEROFTREELIGHTS; j++)
				{
					snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/Lights.%i.%i.png",pixmapPath,prefix,treeLampSet,j+1);
					image=imlib_load_image(pathname);
					if(image!=NULL)
						{
							imlib_context_set_image(image);
							imlib_context_set_drawable(drawOnThis);
							imlib_image_set_has_alpha(1);
							imlib_render_pixmaps_for_whole_image(&treeLampsPixmap[j][ONPIXMAP],&treeLampsPixmap[j][ONMASK]);
							treeLampCount++;
							gotsomelamps=true;
							imlib_free_image();
						}
				}
		}
	if(gotsomelamps==false)
		showTreeLamps=false;

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Star.Off.png",pixmapPath,prefix,treeNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showStar=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&starPixmap[ONPIXMAP],&starPixmap[ONMASK]);
			imlib_free_image();

			snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Star.On.png",pixmapPath,prefix,treeNumber);
			image=imlib_load_image(pathname);
			if(image==NULL)
				showStar=false;
			else
				{
					imlib_context_set_image(image);
					imlib_context_set_drawable(drawOnThis);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&starPixmap[OFFPIXMAP],&starPixmap[OFFMASK]);
					imlib_free_image();
				}
		}

	snprintf(pathname,MAXPATHNAMELEN,"%s/%s/Tree/%i.Tinsel.png",pixmapPath,prefix,treeNumber);
	image=imlib_load_image(pathname);
	if(image==NULL)
		showTinsel=false;
	else
		{
			imlib_context_set_image(image);
			imlib_context_set_drawable(drawOnThis);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&tinselPixmap[ONPIXMAP],&tinselPixmap[ONMASK]);
			imlib_free_image();
		}
}

void destroyLamps(void)
{
	imlib_free_pixmap_and_mask(lampsPixmap[ONPIXMAP]);
	imlib_free_pixmap_and_mask(lampsPixmap[OFFPIXMAP]);
}

void destroyTree(void)
{
	imlib_free_pixmap_and_mask(treePixmap[ONPIXMAP]);
	for(int j=0; j<treeLampCount; j++)
		imlib_free_pixmap_and_mask(treeLampsPixmap[j][ONPIXMAP]);
}

void destroyFigure(void)
{
	imlib_free_pixmap_and_mask(figurePixmap[ONPIXMAP]);
	imlib_free_pixmap_and_mask(figurePixmap[OFFPIXMAP]);
}

void destroyFalling(void)
{
	for(int j=0; j<fallingCount; j++)
		{
			for(int k=0; k<floating[j].anims; k++)
				{
					imlib_free_pixmap_and_mask(*(floating[j].pixmap[k]));
				}
		}
}

void destroyFlyers(void)
{
	for(int j=0; j<flyerCount; j++)
		{
			for(int k=0; k<flyers[j].anims; k++)
				{
					imlib_free_pixmap_and_mask(*(flyers[j].pixmap[k]));
				}
		}
}
