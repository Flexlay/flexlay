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

#include "editor_objmap.hxx"
#include "objmap_object.hxx"
#include "object_add_command.hxx"

ObjectAddCommand::ObjectAddCommand(EditorObjMap* o, ObjMapObject* ob)
  : objmap(o), obj(ob)
{
}

ObjectAddCommand::~ObjectAddCommand()
{
}

int
ObjectAddCommand::get_handle() const
{ 
  return obj->get_handle(); 
}

void
ObjectAddCommand::execute()
{
  objmap->add_object(obj);
}

void
ObjectAddCommand::undo()
{
  objmap->delete_object(obj->get_handle());
}

void
ObjectAddCommand::redo()
{
  execute();
}

std::string
ObjectAddCommand::serialize()
{
  return "";
}

/* EOF */
