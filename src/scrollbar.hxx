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

#ifndef HEADER_SCROLLBAR_HXX
#define HEADER_SCROLLBAR_HXX

#include <ClanLib/GUI/component.h>
#include <ClanLib/Core/Math/rect.h>
#include "shared_ptr.hxx"

class ScrollbarImpl;

/** */
class Scrollbar : public CL_Component
{
public:
  Scrollbar(const CL_Rect& rect, CL_Component* parent);
  
  void set_range(float min, float max);
  void set_pagesize(float size);
  void set_pos(float pos);
private:
  SharedPtr<ScrollbarImpl> impl;
};

#endif

/* EOF */
