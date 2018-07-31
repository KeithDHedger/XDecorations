/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:44:55 BST 2018 keithdhedger@gmail.com

 * This file (startstop.h) is part of XDecorations.

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

#ifndef _STARTSTOP_
#define _STARTSTOP_

void initBottomSnow(void);
void initWindowSnow(void);

void initFlyers(void);
void initFigure(void);
void initLamps(void);
void initFalling(void);
void initTree(void);

void destroyLamps(void);
void destroyTree(void);
void destroyFigure(void);
void destroyFalling(void);
void destroyFlyers(void);

#endif
