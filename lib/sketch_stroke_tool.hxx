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

#ifndef HEADER_SKETCH_STROKE_TOOL_HXX
#define HEADER_SKETCH_STROKE_TOOL_HXX

#include "tool_impl.hxx"

class SketchStrokeToolImpl;

/** */
class SketchStrokeTool
{
private:
public:
  SketchStrokeTool();

  void set_size(float size);
  void set_color(const CL_Color& color);
  Tool to_tool();
private:
  SharedPtr<SketchStrokeToolImpl> impl;
};

#endif

/* EOF */
