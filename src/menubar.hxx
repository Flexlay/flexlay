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

#ifndef HEADER_MENUBAR_HXX
#define HEADER_MENUBAR_HXX

#include <ClanLib/GUI/component.h>
#include <ClanLib/Core/Math/rect.h>
#include "shared_ptr.hxx"

class Menu;
class MenubarImpl;

/** */
class Menubar : public CL_Component
{
public:
  Menubar(const CL_Point& pos, CL_Component* parent);

  void add_submenu(const std::string& name, const Menu& menu);
private:
  SharedPtr<MenubarImpl> impl;
};

#endif

/* EOF */
