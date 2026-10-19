// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/sprite_loader.hpp"

#include "flexlay/pixel_buffer.hpp"
#include "flexlay/util/sexpr.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace flexlay {
namespace supertux {
namespace {

const SExpr* find_child(const SExpr& list, const std::string& tag)
{
  if (!list.is_list()) {
    return nullptr;
  }
  for (const auto& child : list.as_list()) {
    if (child.is_list() && child.tag() == tag) {
      return &child;
    }
  }
  return nullptr;
}

std::string child_atom(const SExpr& list, const std::string& tag, const std::string& def = {})
{
  const SExpr* c = find_child(list, tag);
  if (!c || c->as_list().size() < 2 || !c->as_list()[1].is_atom()) {
    return def;
  }
  return c->as_list()[1].as_atom();
}

int child_int(const SExpr& list, const std::string& tag, int def = 0)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  try {
    return std::stoi(a);
  } catch (...) {
    return def;
  }
}

std::string dirname_of(const std::string& path)
{
  const auto pos = path.find_last_of("/\\");
  if (pos == std::string::npos) {
    return ".";
  }
  if (pos == 0) {
    return "/";
  }
  return path.substr(0, pos);
}

SpriteAction parse_action(const SExpr& node)
{
  SpriteAction act;
  act.name = child_atom(node, "name");
  act.mirror_action = child_atom(node, "mirror-action");
  const SExpr* images = find_child(node, "images");
  if (images && images->is_list()) {
    for (size_t i = 1; i < images->as_list().size(); ++i) {
      if (images->as_list()[i].is_atom()) {
        act.images.push_back(images->as_list()[i].as_atom());
      }
    }
  }
  // Single (image "...") form some older files may use
  const std::string one = child_atom(node, "image");
  if (!one.empty()) {
    act.images.push_back(one);
  }
  return act;
}

} // namespace

LoadedSpriteDef load_sprite_def_text(const std::string& text, const std::string& base_dir)
{
  const SExpr root = sexpr_parse(text);
  if (root.tag() != "supertux-sprite") {
    throw std::runtime_error("sprite_loader: root is not supertux-sprite");
  }
  LoadedSpriteDef def;
  def.version = child_int(root, "version");
  def.base_dir = base_dir;

  const SExpr* actions = find_child(root, "actions");
  if (actions && actions->is_list()) {
    for (size_t i = 1; i < actions->as_list().size(); ++i) {
      const auto& child = actions->as_list()[i];
      if (child.is_list() && child.tag() == "action") {
        def.actions.push_back(parse_action(child));
      }
    }
  }
  return def;
}

LoadedSpriteDef load_sprite_def_file(const std::string& path)
{
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("sprite_loader: cannot open " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return load_sprite_def_text(ss.str(), dirname_of(path));
}

std::optional<std::string> preferred_image_path(const LoadedSpriteDef& def)
{
  auto images_for = [&](const SpriteAction& act) -> const std::vector<std::string>* {
    if (!act.images.empty()) {
      return &act.images;
    }
    if (!act.mirror_action.empty()) {
      for (const auto& other : def.actions) {
        if (other.name == act.mirror_action && !other.images.empty()) {
          return &other.images;
        }
      }
    }
    return nullptr;
  };

  static const char* preferred[] = {"default", "editor", "left", "idle", "normal", "right"};
  for (const char* want : preferred) {
    for (const auto& act : def.actions) {
      if (act.name == want) {
        if (const auto* imgs = images_for(act)) {
          return def.base_dir + "/" + (*imgs)[0];
        }
      }
    }
  }
  for (const auto& act : def.actions) {
    if (const auto* imgs = images_for(act)) {
      return def.base_dir + "/" + (*imgs)[0];
    }
  }
  return std::nullopt;
}

std::optional<Sprite> load_sprite_from_def(const std::string& sprite_path)
{
  try {
    const LoadedSpriteDef def = load_sprite_def_file(sprite_path);
    const auto img = preferred_image_path(def);
    if (!img) {
      return std::nullopt;
    }
    return Sprite(PixelBuffer::from_file(*img), *img);
  } catch (...) {
    return std::nullopt;
  }
}

} // namespace supertux
} // namespace flexlay
