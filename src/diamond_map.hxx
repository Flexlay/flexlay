//  $Id: diamond_map.hxx,v 1.4 2003/09/27 20:57:39 grumbel Exp $
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

#ifndef HEADER_DIAMOND_MAP_HXX
#define HEADER_DIAMOND_MAP_HXX

#include <ClanLib/display.h>
#include "field.hxx"

/** */
class DiamondMap
{
private:
  Field<int> dmap;
  CL_Sprite sprite;
  int num_diamonds;
  int num_max_diamonds;

  void collect(const CL_Vector& pos);
public:
  DiamondMap(Field<int>* d);
  virtual ~DiamondMap();

  void draw ();
  void update (float delta);  

  int get_num_diamonds();
  int get_num_max_diamonds();
};

#endif

/* EOF */
