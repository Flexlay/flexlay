/*  $Id$
**            _   ___              _   
**   _ _  ___| |_| _ )_ _ _  _ _ _| |_ 
**  | ' \/ -_)  _| _ \ '_| || (_-<|   |
**  |_||_\___|\__|___/_|  \_,_/__/|_|_|
**  netBrush - Copyright (C) 2006 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_CLIENT_CONNECTION_HPP
#define HEADER_CLIENT_CONNECTION_HPP

#include <string>
#include "SDL_net.h"

class ClientConnection
{
public:
  int id;
  TCPsocket   tcpsock;
  int         buffer_pos;
  std::string buffer;
  bool        invalid;
  bool        full_client;

public:
  ClientConnection(int id_, TCPsocket socket);
  
  bool is_invalid();
  void update();
  void send_string(const std::string& line);
  void process_line(const std::string& line);
};

#endif

/* EOF */
