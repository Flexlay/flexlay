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

#include <boost/python.hpp>
#include "python_functor.hxx"

PythonFunctor::PythonFunctor()
  : obj(0)
{
}

PythonFunctor::PythonFunctor(boost::python::object o)
{
  obj = o;
}

PythonFunctor::PythonFunctor(const PythonFunctor& copy)
{
  obj = copy.obj;
}

PythonFunctor::~PythonFunctor()
{
}

PythonFunctor&
PythonFunctor::operator=(const PythonFunctor& copy)
{
  if (this != &copy)
    {
      obj = copy.obj;
    }
  return *this;  
}

void
PythonFunctor::operator()()
{
  PyObject* arglist = PyTuple_New(0);
  PyEval_CallObject(obj.ptr(),  arglist);
  Py_DECREF(arglist);
}

/* EOF */
