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

#ifndef HEADER_EDITOR_OBJMAP_HXX
#define HEADER_EDITOR_OBJMAP_HXX

#include <vector>
#include <ClanLib/Display/sprite.h>
#include <ClanLib/GUI/component.h>
#include <ClanLib/Core/Math/point.h>

/** GUI Component that holds positioned objects, ie. objects which
    consist of a CL_Sprite and some properties */
class EditorObjMap : public CL_Component
{
private:
  CL_SlotContainer slots;

  struct Obj {
    CL_Sprite sprite;
    CL_Point  pos;
  };

  typedef std::vector<Obj> Objs;
  Objs objects;
public:
  EditorObjMap(CL_Component* parent);
  ~EditorObjMap();

  void update(float delta);
  void draw();

  void mouse_up  (const CL_InputEvent& event);
  void mouse_down(const CL_InputEvent& event);
  void mouse_move(const CL_InputEvent& event);
private:
  EditorObjMap (const EditorObjMap&);
  EditorObjMap& operator= (const EditorObjMap&);
};

#endif

/* EOF */