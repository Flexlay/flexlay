//  $Id$
// 
//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_GRAPHIC_CONTEXT_STATE_HXX
#define HEADER_GRAPHIC_CONTEXT_STATE_HXX

#include <ClanLib/Core/Math/point.h>
#include <ClanLib/Core/Math/rect.h>
#include "shared_ptr.hxx"

class CL_GraphicContext;

class GraphicContextStateImpl;

/** Helper class for capturing the state of a GraphicContext, with
    additional convenience functions to make handling GraphicContexts
    easier */
class GraphicContextState
{
public:
  GraphicContextState();
  GraphicContextState(int w, int h);

  void set_size(int w, int h);

  void push(CL_GraphicContext* gc = 0);
  void pop (CL_GraphicContext* gc = 0);

  /** Return a rectangle in world coordinates that represents the area
      visible on the screen */
  CL_Rect get_clip_rect();

  int get_width()  const;
  int get_height() const;

  void      set_pos(const CL_Pointf& pos);
  CL_Pointf get_pos() const;

  /** Set zoom to z, while preserving the screen position pos at the same point */
  void  set_zoom(CL_Pointf pos, float z);
  void  set_zoom(float z);
  float get_zoom(); 

  void zoom_to (const CL_Rectf& rect);

  CL_Pointf screen2world(const CL_Point& pos);

private:
  SharedPtr<GraphicContextStateImpl> impl;
};

#endif

/* EOF */
