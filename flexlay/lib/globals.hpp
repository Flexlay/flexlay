//  $Id: globals.hpp,v 1.11 2003/11/07 13:00:39 grumbel Exp $
// 
//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GLOBALS_HXX
#define GLOBALS_HXX

#include <string>
#include <ClanLib/Core/Resources/resource_manager.h>

/** datadir => /usr/local/share/games/windstille/ */
extern std::string datadir;

/** bindir => /usr/local/games/ */
extern std::string bindir;

/** homedir => /home/juser/.windstille/ */
extern std::string homedir;

extern int   debug;

#endif

/* EOF */
