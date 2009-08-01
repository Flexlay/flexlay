//  $Id$
//
//  Flexlay - A Generic 2D Game Editor
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#include "python_object.hxx"

PythonObject::PythonObject(PyObject* obj_)
  : obj(obj_)
{
  Py_XINCREF(obj);
}

PythonObject::PythonObject (const PythonObject& copy)
  : obj(copy.obj)
{
  Py_XINCREF(obj);
}

PythonObject& 
PythonObject::operator= (const PythonObject& copy)
{
  if (this != &copy)
    {
      Py_XDECREF(obj);
      obj = copy.obj;
      Py_XINCREF(obj);
    }
  return *this;
}

PythonObject::~PythonObject()
{
  Py_XDECREF(obj);
}

/* EOF */
