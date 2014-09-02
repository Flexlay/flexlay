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

#include "graphic_context_state.hpp"

#include <ClanLib/Display/display.h>
#include <ClanLib/Display/display_window.h>
#include <ClanLib/Display/graphic_context.h>

class GraphicContextStateImpl
{
public:
  int width;
  int height;

  Pointf offset;
  float zoom;
  float rotation;
};

GraphicContextState::GraphicContextState()
  : impl(new GraphicContextStateImpl())
{
  impl->width  = 1;
  impl->height = 1;
  impl->offset = Pointf(0,0);
  impl->zoom   = 1.0f;
  impl->rotation = 0;
}

GraphicContextState::GraphicContextState(int w, int h)
  : impl(new GraphicContextStateImpl())
{
  impl->width  = w;
  impl->height = h;
  impl->offset = Pointf(0,0);
  impl->zoom   = 1.0f;
  impl->rotation = 0;
}

void
GraphicContextState::set_size(int w, int h)
{
  impl->width  = w;
  impl->height = h;
}

void
GraphicContextState::push(CL_GraphicContext* gc) const
{
  if (gc == 0)
    gc = CL_Display::get_current_window()->get_gc();

  gc->push_modelview();

  gc->add_translate(impl->width/2, impl->height/2);
  gc->add_rotate(impl->rotation, 0, 0, 1.0);
  gc->add_translate(-impl->width/2, -impl->height/2);

  gc->add_scale(get_zoom(), get_zoom());
  gc->add_translate(impl->offset.x, impl->offset.y);
}

void
GraphicContextState::pop(CL_GraphicContext* gc) const
{
  if (gc == 0)
    gc = CL_Display::get_current_window()->get_gc();

  gc->pop_modelview();
}

Rectf
GraphicContextState::get_clip_rect() const
{
  return Rectf(Pointf(-impl->offset.x,
                            -impl->offset.y),
                  Sizef(get_width()  / impl->zoom,
                           get_height() / impl->zoom));
}

void
GraphicContextState::set_pos(const Pointf& pos)
{
  impl->offset.x = -pos.x + (get_width()/2  / impl->zoom);
  impl->offset.y = -pos.y + (get_height()/2 / impl->zoom);
}

Pointf
GraphicContextState::get_pos() const
{
  return Pointf(-impl->offset.x + (get_width()/2  / impl->zoom),
                   -impl->offset.y + (get_height()/2  / impl->zoom));
}

void
GraphicContextState::set_zoom(Pointf pos, float z)
{
  float old_zoom = impl->zoom;
  set_zoom(z);
  impl->offset.x = pos.x/impl->zoom - pos.x/old_zoom + impl->offset.x;
  impl->offset.y = pos.y/impl->zoom - pos.y/old_zoom + impl->offset.y;
}

void
GraphicContextState::set_zoom(float z)
{
  impl->zoom = z;
}

float
GraphicContextState::get_zoom() const
{
  return impl->zoom;
}

void
GraphicContextState::zoom_to (const Rectf& rect)
{
  float center_x = (rect.left + rect.right) / 2.0f;
  float center_y = (rect.top + rect.bottom) / 2.0f;

  float width  = rect.right - rect.left;
  float height = rect.bottom - rect.top;
  float screen_relation = float(get_height())/float(get_width ());
  float rect_relation   = height/width;

  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
  {
    impl->zoom = get_width()/width;
  }
  else // take height, ignore width
  {
    impl->zoom = get_height()/height;
  }

  impl->offset.x = (get_width()  / (2*impl->zoom)) - center_x;
  impl->offset.y = (get_height() / (2*impl->zoom)) - center_y;
}

Pointf
GraphicContextState::screen2world(const Point& pos_)
{
  Pointf pos = pos_;
  float sa = sin(-impl->rotation/180.0f*M_PI);
  float ca = cos(-impl->rotation/180.0f*M_PI);

  float dx = pos.x - impl->width/2;
  float dy = pos.y - impl->height/2;

  pos.x = impl->width/2  + (ca * dx - sa * dy);
  pos.y = impl->height/2 + (sa * dx + ca * dy);

  Pointf p((float(pos.x) / impl->zoom) - impl->offset.x,
              (float(pos.y) / impl->zoom) - impl->offset.y);

  return p;
}

void
GraphicContextState::set_rotation(float angle)
{
  impl->rotation = angle;
}

float
GraphicContextState::get_rotation() const
{
  return impl->rotation;
}

int
GraphicContextState::get_width()  const
{
  return impl->width;
}

int
GraphicContextState::get_height() const
{
  return impl->height;
}

/* EOF */
