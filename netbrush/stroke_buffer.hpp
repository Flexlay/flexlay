/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_STROKE_BUFFER_HPP
#define HEADER_STROKE_BUFFER_HPP

class Rect;
class DrawingParameter;
class Dab;

/** */
class StrokeBuffer
{
private:
  GrayscaleBuffer*  buffer;
  Stroke*           stroke;
  DrawingParameter* param;

public:
  StrokeBuffer(int w, int h);
  ~StrokeBuffer();

  void set_param(DrawingParameter* param);

  void add_dab(const Dab& dab);
  void clear();
  void clear(const Rect& rect);

  void draw(SDL_Surface* target, const Rect& rect, int x_of, int y_of);

  void draw_stroke(const Stroke& stroke, DrawingParameter* param);

private:
  StrokeBuffer (const StrokeBuffer&);
  StrokeBuffer& operator= (const StrokeBuffer&);
};

#endif

/* EOF */
