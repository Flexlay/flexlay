//  $Id$
// 
//  Pingus - A free Lemmings clone
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

#ifndef HEADER_ZOOM_TOOL_HXX
#define HEADER_ZOOM_TOOL_HXX

#include <ClanLib/Core/Math/rect.h>
#include <ClanLib/Display/input_event.h>
#include "tilemap_tool.hxx"

/** */
class ZoomTool : public TileMapTool
{
private:
  enum { CREATE_ZOOM_RECT, NONE } state;

  CL_Rect zoom_rect;
public:
  ZoomTool();
  ~ZoomTool();
  
  void draw();

  void on_mouse_up  (const CL_InputEvent& event);
  void on_mouse_down(const CL_InputEvent& event);
  void on_mouse_move(const CL_InputEvent& event);
 
private:
  ZoomTool (const ZoomTool&);
  ZoomTool& operator= (const ZoomTool&);
};

#endif

/* EOF */
