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

#include <iostream>
#include <sstream>
#include <fstream>
#include <ClanLib/Display/palette.h>
#include <ClanLib/Display/sprite.h>
#include <ClanLib/Display/sprite_description.h>
#include <ClanLib/Display/Providers/provider_factory.h>
#include "globals.hxx"
#include "tile_provider.hxx"
#include "tile.hxx"
#include "tileset.hxx"
#include "tilemap_layer.hxx"
#include "editor_map.hxx"
#include "netpanzer.hxx"

CL_Palette netpanzer_load_palette(const char* filename)
{
  CL_Palette palette;
  unsigned char color_array[256 * 3];

  std::ifstream in(filename);

  if (!in)
    {
      std::cout << "Couldn't load palette" << std::endl;
      return palette;
    }

  in.read(reinterpret_cast<char*>(color_array), sizeof(color_array));

  for(int i = 0; i < 256; ++i)
    {
      palette.colors[i].set_red  (color_array[3*i + 0]);
      palette.colors[i].set_green(color_array[3*i + 1]);
      palette.colors[i].set_blue (color_array[3*i + 2]);
    }

  return palette;
}

unsigned char find_nearest_color(const CL_Palette& palette, const CL_Color& rgb)
{ // Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue
  float bestDist = 10000000.0f;
  int   best     = 0;

  float vPic = sqrt(rgb.get_red() * rgb.get_red() 
                    + rgb.get_green() * rgb.get_green()
                    + rgb.get_blue() * rgb.get_blue()) * 0.57735027;
 
  for (int i = 0; i < 256; i++) {
    float vPal = sqrt(palette.colors[i].get_red()     * palette.colors[i].get_red()
                      + palette.colors[i].get_green() * palette.colors[i].get_green()
                      + palette.colors[i].get_blue()  * palette.colors[i].get_blue()) * 0.57735027;

    float dr = palette.colors[i].get_red()   - rgb.get_red();
    float dg = palette.colors[i].get_green() - rgb.get_green();
    float db = palette.colors[i].get_blue()  - rgb.get_blue();
    float dv = vPal-vPic;
    float dist = dr * dr * 0.3 + dg * dg * 0.59 + db * db * 0.11 + dv * dv * 0.7;

    if (dist < bestDist) {
      bestDist = dist;
      best = i;
    }
  }

  return best;
}

class NetPanzerFileStructImpl
{
public:
  std::string id_header;
  std::string name;
  std::string description;
  TilemapLayer tilemap; 
  Tileset tileset;
};

std::string
NetPanzerFileStruct::get_id_header()
{
  return impl->id_header;
}

std::string
NetPanzerFileStruct::get_name()
{
  return impl->name;
}

std::string
NetPanzerFileStruct::get_description()
{
  return impl->description;
}

TilemapLayer
NetPanzerFileStruct::get_tilemap()
{
  return impl->tilemap;
}

void
NetPanzerFileStruct::set_id_header(const std::string& id)
{
  impl->id_header = id;
}

void
NetPanzerFileStruct::set_name(const std::string& name)
{
  impl->name = name;
}

void
NetPanzerFileStruct::set_description(const std::string& description)
{
  impl->description = description;
}

void
NetPanzerFileStruct::set_tilemap(TilemapLayer l)
{
  impl->tilemap = l;
}


void
NetPanzerFileStruct::save(const std::string& filename)
{
  if (impl->tilemap.is_null())
    return;
    
  unsigned char   netp_id_header[64];
  strcpy(reinterpret_cast<char*>(netp_id_header), impl->id_header.c_str());
  unsigned short  id       = 0; // ?
  char   name[256];
  strcpy(name, impl->name.c_str());
  char   description[1024];
  strcpy(description, impl->description.c_str());
  unsigned short  x_size   = impl->tilemap.get_width();
  unsigned short  y_size   = impl->tilemap.get_height();
  char            tile_set[256] = "summer12mb.tls";
 
  unsigned short  thumbnail_x_pix = impl->tilemap.get_width();
  unsigned short  thumbnail_y_pix = impl->tilemap.get_height();
    
  std::ofstream out(filename.c_str());

  out.write(reinterpret_cast<char*>(&netp_id_header), sizeof(netp_id_header));
  out.write(reinterpret_cast<char*>(&id), sizeof(short));
  out.write(reinterpret_cast<char*>(&name), sizeof(name));
  out.write(reinterpret_cast<char*>(&description), sizeof(description));
  out.write(reinterpret_cast<char*>(&x_size), sizeof(short));
  out.write(reinterpret_cast<char*>(&y_size), sizeof(short));
  out.write(reinterpret_cast<char*>(&tile_set), sizeof(tile_set));
  out.write(reinterpret_cast<char*>(&thumbnail_x_pix), sizeof(short));
  out.write(reinterpret_cast<char*>(&thumbnail_y_pix), sizeof(short));

  std::vector<unsigned short> vec(x_size * y_size);

  Field<int>* field = impl->tilemap.get_field();
  for(int i = 0; i < x_size * y_size; ++i)
    {
      vec[i] = (*field)[i];
    }
  out.write(reinterpret_cast<char*>(&(*vec.begin())), 
            sizeof(unsigned short)*vec.size());

  // Generate thumbnail
  CL_Palette palette = netpanzer_load_palette("netp.act");
  
  std::vector<unsigned char> thumbnail(x_size * y_size);
  for(int i = 0; i < int(thumbnail.size()); ++i)
    {
      Tile* tile = impl->tileset.create((*field)[i]);
      if (tile)
        thumbnail[i] = find_nearest_color(palette, tile->get_color());
    }

  out.write(reinterpret_cast<char*>(&(*thumbnail.begin())), 
            sizeof(unsigned char)*thumbnail.size());
}

NetPanzerFileStruct::NetPanzerFileStruct(Tileset tileset, int w, int h)
  : impl(new NetPanzerFileStructImpl())
{
  impl->tileset = tileset;
  impl->id_header = "<Id Header>";
  impl->name = "<Name>";
  impl->description = "<Description>";
  impl->tilemap = TilemapLayer(tileset, w, h);
}

NetPanzerFileStruct::NetPanzerFileStruct(Tileset tileset, const std::string& filename)
  : impl(new NetPanzerFileStructImpl())
{
  impl->tileset = tileset;
  
  // FIXME: endian issues
  unsigned char   netp_id_header[64]; // Copyright PyroSoft Inc.  ...
  unsigned short  id; // What is this?
  char            name[256];
  char            description[1024];
  unsigned short  x_size; // width
  unsigned short  y_size; // height
  char            tile_set[256]; // name of the tileset: "summer12mb.tls"
 
  unsigned short  thumbnail_x_pix;
  unsigned short  thumbnail_y_pix;

  std::ifstream file(filename.c_str());

  if (!file)
    {
      std::cout << "NetPanzerFileStructImpl: Error: " << filename << std::endl;
    }

  file.read(reinterpret_cast<char*>(&netp_id_header), sizeof(netp_id_header));
  file.read(reinterpret_cast<char*>(&id), sizeof(short));
  file.read(reinterpret_cast<char*>(&name), sizeof(name));
  file.read(reinterpret_cast<char*>(&description), sizeof(description));
  file.read(reinterpret_cast<char*>(&x_size), sizeof(short));
  file.read(reinterpret_cast<char*>(&y_size), sizeof(short));
  file.read(reinterpret_cast<char*>(&tile_set), sizeof(tile_set));
  file.read(reinterpret_cast<char*>(&thumbnail_x_pix), sizeof(short));
  file.read(reinterpret_cast<char*>(&thumbnail_y_pix), sizeof(short));

  TilemapLayer tilemap(tileset, x_size, y_size);
  Field<int>* field      = tilemap.get_field();

  std::vector<unsigned short> vec;
  vec.resize(x_size * y_size);
  file.read(reinterpret_cast<char*>(&(*vec.begin())), sizeof(unsigned short)*vec.size());

  for(int i = 0; i < x_size*y_size; ++i)
    (*field)[i] = vec[i];

  std::cout << "Thumbnail: " << thumbnail_x_pix << " " << thumbnail_y_pix << std::endl;

  impl->tilemap     = tilemap;
  impl->id_header   = reinterpret_cast<char*>(netp_id_header);
  impl->name        = name;
  impl->description = description;
}

class NetPanzerTileProviderImpl
{
private:
  int id;
  CL_Sprite sprite;

public:
  NetPanzerTileProviderImpl(int id_)
    : id(id_)
  {    
  }

  CL_Sprite get_sprite()
  {
    if (sprite)
      {
        return sprite;
      }
    else
      {
        char str[1024];
        sprintf(str, "netpanzertiles/tile%04d.png", id); 
        
        CL_SpriteDescription desc;
        desc.add_frame(CL_ProviderFactory::load(str));
        sprite = CL_Sprite(desc);
        return sprite;
      }
  }
};

void load_netpanzer_tiles(Tileset tileset)
{
  char str[1024];
  for(int i = 0; i <= 11960; ++i)
    {
      sprintf(str, "netpanzertiles/tile%04d.png", i);
      Tile* tile = new Tile(str, CL_Color(255,   0,   0, 128));
      tileset.add_tile(i, tile);
      delete tile;
    }
}

/* EOF */
