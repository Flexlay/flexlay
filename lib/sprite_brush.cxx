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

#include "brush_impl.hxx"
#include "sprite_brush.hxx"

class SpriteBrushImpl : public BrushImpl
{
public:
  CL_Sprite sprite;
  
  SpriteBrushImpl(const CL_Sprite& sprite_)
    : sprite(sprite_)
  {
    sprite.set_alignment (origin_center, 0, 0);
  }

  virtual ~SpriteBrushImpl()
  {
  }

  CL_Sprite get_sprite() 
  {
    return sprite;
  }

  BrushImpl* clone() const 
  {
    return new SpriteBrushImpl(sprite);
  }
};

SpriteBrush::SpriteBrush(const CL_Sprite& sprite_)
  : impl(new SpriteBrushImpl(sprite_))
{
}

Brush
SpriteBrush::to_brush()
{
  return Brush(impl);
}

/* EOF */
