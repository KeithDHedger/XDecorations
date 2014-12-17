/******************************************************
*
*     ©keithhedger Wed 17 Dec 11:18:33 GMT 2014
*     kdhedger68713@gmail.com
*
*     routines.cpp
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
