//  $Id: tilemap_paint_tool.cxx,v 1.2 2003/09/23 22:07:32 grumbel Exp $
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

#include <iostream>
#include <ClanLib/Display/mouse.h>
#include <ClanLib/Display/keyboard.h>
#include <ClanLib/Display/keys.h>
#include <ClanLib/Display/display.h>
#include "globals.hxx"
#include "editor_tilemap.hxx"
#include "tileset.hxx"
#include "editor_map.hxx"
#include "editor_map_component.hxx"
#include "tile.hxx"
#include "editor.hxx"
#include "workspace.hxx"
#include "paint_command.hxx"
#include "editor_names.hxx"
#include "tilemap_paint_tool.hxx"

TileMapPaintTool* TileMapPaintTool::current_ = 0; 

TileMapPaintTool::TileMapPaintTool()
{
  last_draw = CL_Point(-1, -1);

  current_  = this;
  brush = TileBrush(1, 1);
  brush.at(0, 0) = 0;
  brush.set_opaque();
  current_tile = CL_Point(0,0);

  command = 0;

  mode = NONE;
  tilemap = 0;
}

TileMapPaintTool::~TileMapPaintTool()
{
}

void
TileMapPaintTool::draw()
{
  if (!tilemap)
    return;

  switch(mode)
    {
    case SELECTING:
      if (CL_Keyboard::get_keycode(CL_KEY_LSHIFT))
        selection.draw(CL_Color(255,  128, 128, 100));
      else 
        selection.draw();
      break;
      
    default:
      int tile_size = tilemap->get_tile_size();

      // Draw the brush:
      for(int y = 0; y < brush.get_height(); ++y)
        for(int x = 0; x < brush.get_width(); ++x)
          {
            Tile* tile = Tileset::current()->create(brush(x, y));
                
            if (tile)
              {
                CL_Sprite sprite = tile->get_sprite();
                sprite.set_alpha(0.5f);
                sprite.draw((current_tile.x + x) * tile_size, 
                            (current_tile.y + y) * tile_size);

                CL_Display::fill_rect(CL_Rect(CL_Point((current_tile.x + x) * tile_size, 
                                                       (current_tile.y + y) * tile_size),
                                              CL_Size(tile_size, tile_size)),
                                      CL_Color(255, 255, 255, 100));
              }
            else if (brush.is_opaque())
              {
                CL_Display::fill_rect(CL_Rect(CL_Point((current_tile.x + x) * tile_size, 
                                                       (current_tile.y + y) * tile_size),
                                              CL_Size(tile_size, tile_size)),
                                      CL_Color(255, 255, 255, 100));
              }
            else
              {
                CL_Display::fill_rect(CL_Rect(CL_Point((current_tile.x + x) * tile_size, 
                                                       (current_tile.y + y) * tile_size),
                                              CL_Size(tile_size, tile_size)),
                                      CL_Color(255, 255, 255, 50));
              }
          }
      break;
    }
}

void
TileMapPaintTool::on_mouse_down(const CL_InputEvent& event)
{
  if (tilemap)
    {
      EditorMapComponent* parent = EditorMapComponent::current();
      CL_Point pos = tilemap->world2tile(parent->screen2world(event.mouse_pos));

      switch (mode)
        {
        case NONE:
          switch (event.id)
            {
            case CL_MOUSE_LEFT:
              mode = PAINTING;
              parent->capture_mouse();
              command = new PaintCommand(tilemap->get_field(), brush);
              command->add_point(pos);

              tilemap->draw_tile(brush, pos);
              last_draw = pos;
              break;
    
            case CL_MOUSE_RIGHT:
              mode = SELECTING;
              parent->capture_mouse();

              selection.start(tilemap, pos);
              break;
            }
          break;

        default:
          break;
        }
    }
}
 
void
TileMapPaintTool::on_mouse_move(const CL_InputEvent& event)
{
  if (tilemap)
    {
      EditorMapComponent* parent = EditorMapComponent::current();
      current_tile = tilemap->world2tile(parent->screen2world(event.mouse_pos));

      switch (mode)
        {
        case PAINTING:
          if (current_tile != last_draw)
            {
              command->add_point(current_tile);
              tilemap->draw_tile(brush, current_tile);
              last_draw = current_tile;
            }
          break;
    
        case SELECTING:
          selection.update(current_tile);
          break;
      
        default:
          break;
        }
    }
}

void
TileMapPaintTool::on_mouse_up  (const CL_InputEvent& event)
{
  if (tilemap)
    {
      EditorMapComponent::current()->get_workspace()->get_current_map()->modify();

      EditorMapComponent* parent = EditorMapComponent::current();
      CL_Point pos = tilemap->world2tile(parent->screen2world(event.mouse_pos));

      switch (event.id)
        {
        case CL_MOUSE_LEFT:
          if (mode == PAINTING)
            {
              parent->release_mouse();
              mode = NONE;

              command->add_point(pos);
              Editor::current()->execute(command);
              command = 0;

              tilemap->draw_tile(brush, pos);
              last_draw = CL_Point(-1, -1);
            }
          break;
    
        case CL_MOUSE_RIGHT:
          if (mode == SELECTING)
            {
              parent->release_mouse();
              mode = NONE;

              selection.update(pos);
              brush = selection.get_brush(*tilemap->get_field());

              if ((brush.get_width() > 1 || brush.get_height() > 1)
                  && !CL_Keyboard::get_keycode(CL_KEY_LSHIFT))
                {
                  brush.set_transparent();
                  brush.auto_crop();
                }
              else
                {
                  brush.set_opaque();
                }

              selection.clear();
            }
          break;
        }
    }
}

void
TileMapPaintTool::set_brush(const TileBrush& b)
{
  brush = b;
}

/* EOF */
