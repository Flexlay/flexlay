// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/place_object_tool.hpp"

#include "flexlay/commands/object_add_command.hpp"
#include "flexlay/color.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/supertux/object_sprites.hpp"
#include "flexlay/supertux/sprite_loader.hpp"

#include <cmath>
#include <filesystem>

namespace flexlay {
namespace {

std::string resolve_sprite_path(const std::string& datadir, const std::string& rel)
{
  if (rel.empty()) {
    return {};
  }
  namespace fs = std::filesystem;
  std::string r = rel;
  if (r.size() >= 8 && r.compare(0, 8, "/images/") == 0) {
    r = r.substr(8);
  } else if (r.size() >= 7 && r.compare(0, 7, "images/") == 0) {
    r = r.substr(7);
  }
  if (!datadir.empty()) {
    const fs::path p = fs::path(datadir) / "images" / r;
    if (fs::is_regular_file(p)) {
      return p.string();
    }
  }
  if (fs::is_regular_file(r)) {
    return r;
  }
  return {};
}

} // namespace

void PlaceObjectTool::set_datadir(std::string datadir)
{
  m_datadir = std::move(datadir);
  reload_preview();
}

void PlaceObjectTool::clear_prototype()
{
  m_proto.reset();
  m_preview_sprite.reset();
  m_preview_hitbox.reset();
}

void PlaceObjectTool::set_prototype(ObjectPrototype proto)
{
  m_proto = std::move(proto);
  reload_preview();
}

void PlaceObjectTool::reload_preview()
{
  m_preview_sprite.reset();
  m_preview_hitbox.reset();
  if (!m_proto) {
    return;
  }
  std::string img = m_proto->editor_image;
  if (img.empty()) {
    if (auto info = supertux::object_visual_info(m_proto->type_id)) {
      img = info->editor_image;
    }
  }
  if (img.empty() || m_datadir.empty()) {
    return;
  }
  const std::string path = resolve_sprite_path(m_datadir, img);
  if (path.empty()) {
    return;
  }
  try {
    if (path.size() >= 7 && path.compare(path.size() - 7, 7, ".sprite") == 0) {
      const auto def = supertux::load_sprite_def_file(path);
      m_preview_hitbox = supertux::preferred_hitbox(def);
      if (auto spr = supertux::load_sprite_from_def(path)) {
        m_preview_sprite = std::move(*spr);
      }
    } else {
      auto buf = PixelBuffer::from_file(path);
      if (!buf.empty()) {
        m_preview_sprite = Sprite(std::move(buf));
      }
    }
  } catch (...) {
  }
}


Pointf PlaceObjectTool::snap_pos(const Pointf& p) const
{
  if (!m_snap || m_snap_size <= 0.0f) {
    return p;
  }
  return Pointf(std::floor(p.x / m_snap_size + 0.5f) * m_snap_size,
                std::floor(p.y / m_snap_size + 0.5f) * m_snap_size);
}

void PlaceObjectTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  m_cursor = snap_pos(world_pos);
  m_have_cursor = true;

  if (button == 2) {
    if (on_cancelled) {
      on_cancelled();
    }
    return;
  }
  if (button != 1 || !m_layer || !m_proto) {
    return;
  }

  auto obj = m_proto->instantiate(snap_pos(world_pos), m_datadir);
  m_layer->clear_selection();
  auto cmd = std::make_shared<ObjectAddCommand>(m_layer);
  cmd->add_object(obj);
  if (auto* map = workspace.map()) {
    map->execute(cmd);
  }
  obj->set_selected(true);
  if (on_placed) {
    on_placed(obj);
  }
}

void PlaceObjectTool::on_mouse_move(Workspace& /*workspace*/, const Pointf& world_pos, int /*buttons*/)
{
  m_cursor = snap_pos(world_pos);
  m_have_cursor = true;
}

void PlaceObjectTool::on_mouse_up(Workspace& /*workspace*/, const Pointf& /*world_pos*/, int /*button*/)
{
}

void PlaceObjectTool::draw(GraphicContext& gc, Workspace& /*workspace*/)
{
  if (!m_have_cursor || !m_proto) {
    return;
  }
  const Sizef sz = m_proto->default_size;

  // Ghost sprite
  if (m_preview_sprite) {
    // Draw via pixel buffer as translucent rect if Sprite has no alpha draw helper
    // Prefer drawing the sprite at cursor; fallback to tinted rect
    try {
      m_preview_sprite->draw(m_cursor.x, m_cursor.y, gc);
    } catch (...) {
    }
  } else {
    Color c = m_proto->default_color;
    c.a = 100;
    gc.fill_rect(Rectf(m_cursor.x, m_cursor.y, m_cursor.x + sz.width, m_cursor.y + sz.height), c);
  }

  // Outer bounds
  gc.draw_rect(Rectf(m_cursor.x, m_cursor.y, m_cursor.x + sz.width, m_cursor.y + sz.height),
               Color(255, 255, 255, 180));

  // Hitbox guide (sprite-local → world)
  if (m_preview_hitbox) {
    const Rectf& hb = *m_preview_hitbox;
    const Rectf world_hb(m_cursor.x + hb.left, m_cursor.y + hb.top,
                         m_cursor.x + hb.right, m_cursor.y + hb.bottom);
    gc.fill_rect(world_hb, Color(0, 255, 180, 50));
    gc.draw_rect(world_hb, Color(0, 255, 180, 220));
  }
}

} // namespace flexlay
