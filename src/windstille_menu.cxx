//  $Id: windstille_menu.cxx,v 1.1 2003/09/29 19:29:17 grumbel Exp $
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
#include <ClanLib/Display/display.h>
#include <ClanLib/Display/keys.h>
#include <ClanLib/Display/keyboard.h>
#include "globals.hxx"
#include "fonts.hxx"
#include "windstille_menu.hxx"

WindstilleMenu::WindstilleMenu()
  : background("menu_background", resources),
    windstille("logo_large", resources)
{
  windstille.set_alignment(origin_top_center);
}

WindstilleMenu::~WindstilleMenu()
{
  
}

void
WindstilleMenu::update(float delta)
{
  background.update(delta);
  windstille.update(delta);

  if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    quit();
}

void
WindstilleMenu::draw()
{
  //std::cout << "Draw... " << std::endl;
  background.draw(0,0);
  windstille.draw(CL_Display::get_width()/2,
                  50);
  
  Fonts::menu.set_alignment(origin_bottom_center);
  Fonts::menu_h.set_alignment(origin_bottom_center);
  Fonts::menu_h.draw(580, 330, "[Start Game]");
  Fonts::menu.draw(580, 385, "Start Editor");
  Fonts::menu.draw(580, 440, "Quit");
  
  Fonts::copyright.set_alignment(origin_bottom_left);
  Fonts::copyright.draw(15, CL_Display::get_height() - 10,
                        PACKAGE_STRING
                        "Copyright (c) 2003 Ingo Ruhnke <grumbel@gmx.de>\n"
                        "Windstille comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome\n"
                        "to redistribute it under certain conditions; see the file COPYING for details.\n");

  CL_Display::flip();
}

/* EOF */
