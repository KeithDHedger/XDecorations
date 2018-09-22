/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:45:43 BST 2018 keithdhedger@gmail.com

 * This file (update.h) is part of XDecorations.

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

#ifndef _UPDATE_
#define _UPDATE_

void eraseRects(void);
void clearBottomSnow(void);
void doGusts(void);
void updateGusts(void);
void updateFalling(void);
void updateLamps(void);
void updateTreeLamps(void);
void updateStar(void);
void updateFigure(void);
void updateFlyers(void);
void getOpenwindows(void);
void clearWindowSnow(int winid);
void clearAllWindowSnow(bool newrsrc);

#endif
